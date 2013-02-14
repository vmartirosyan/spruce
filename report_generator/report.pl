#!/usr/bin/perl -w 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

use warnings;

use HTML::Template; # For template-generation output files.

use XML::Parser; # For parse Spruce logs.
use Getopt::Long; # For parse options

#---------------------------------------------------------------------
# Terminate program as with fatal error.
#
# Arguments:
#
# $1 - message for output before termination.
sub fail {
    my $text = shift;
    
    print STDERR "$text\n";
    exit(-1);
}

# Parameters.
my $script_directory; # Where script is located
my $problem_db_filename; # File with problem db
my $result_dir; # Directory for store results
my $journals_dir; # Directory with test journals.
my $known_are_quelled;

# Ensures that the specified path is absolute, prepending the current
# working directory to relative paths.
sub make_path_absolute
{
    my $input_path = shift;
    # While Perl has 'readlink' function, one cannot affect on its arguments.
    # So use common backtics for call Linux readlink command and extract its output.
    my $abs_path = `readlink -mnq $input_path`;
    
    return $abs_path;
}

sub BEGIN {
    $| = 1; # Immediate flush of all output

    # Detect script directory.
    $script_directory = $0;
    $script_directory =~ s/[^\/]*$//; # Basename
    # When run program as "report.pl", basename is "" (empty string). Process this case as ".".
    if($script_directory eq "")
    {
        $script_directory = ".";
    }

    $script_directory = make_path_absolute($script_directory);
    # Append the script directory to the @INC to attach our own modules
    push @INC, $script_directory;
}

use strict;

use IO::File;

# HTML colors used in the report(now included directly into template).
my $dark_gray = "rgb(64, 64, 64)";
my $gray = "rgb(200, 200, 200)";
my $neutral_gray = "rgb(230, 230, 230)";
my $bright_gray = "rgb(240, 240, 240)";
my $fluorescent_green = "rgb(64, 255, 64)";
my $fluorescent_yellow = "rgb(255, 255, 64)";
my $fluorescent_red = "rgb(255, 96, 96)";
my $pale_blue = "rgb(220, 220, 240)";
my $dark_blue = "rgb(0, 0, 128)";
my $dark_red = "rgb(128, 0, 0)";
my $red = "rgb(255, 0, 0)";
my $green = "rgb(0, 160, 0)";
my $blue = "rgb(0, 0, 255)";

# 'Journal' is a reference to hash, contained:
# 1. Common info
#       - journal_title
#       - journal_anchor
#       - filename [currently not used]
#       - test_version
# 2. Statistic
#       - stats_tests_total
#       - stats_tests_passed
#       - stats_tests_skipped
#       - stats_tests_quelled
#       - stats_tests_failed
#       - stats_tests_failed_known
# 3. Grouping info
#       - group
#       - subgroup
# 4. Problems: array of hash references to problems descriptions.
#       - problems
# 5. Information: array of hash references to journal info.
#       - infos
# 6. Status
#       - status
#       - status_{success, failed, warning}

# Problem is a reference to hash, contained:
#   - problem_id(auto-assigned)
#   - test_name
#   - test_point
#   - test_number
#   - severity
#   - messages
#   - quelled
#   - waived
#   - resolution
#   - comment
#   - lsb_report_id
#   - lsb_resolution_id


# Title info.

# Name of the testing system.
my $test_system;
# Name of the system under test.
my $system_under_test;

# System common information (about system under test, tests common version...).
# This map is used for match properties in problem database.
my %system_info = ();

# Array of global information for output.
#
# Each element is hash with fields
#   - title
#   - content
my @test_infos = ();

# Add common tests information.
#
# Arguments:
#
# $1 - title.
# $2 - content.
sub add_test_info($$)
{
    my ($title, $content) = @_;
    
    my $test_info = {
        'title' => $title,
        'content' => $content,
    };
    
    push (@test_infos, $test_info);
}

# Summary statistics
my $tests_total = 0;
my $tests_passed = 0;
my $tests_skipped = 0;
my $tests_quelled = 0;
my $tests_failed = 0;
my $tests_failed_known = 0;


# Problems which is not concerned with tests(e.g., journal parse error).
#
# Problem is a reference to hash, contained:
#   - location
#   - serverity
#   - comment
#
my @other_problems = ();

# Add problem record not about a test.
#
# Arguments:
#
# 
# $1 - location.
# $2 - severity.
# $3 - comment.
sub add_problem_other($$$)
{
    my ($location, $severity, $comment) = @_;
    
    my $problem = {
        'location' => $location,
        'severity' => $severity,
        'comment' => $comment,
    };  
    
    push (@other_problems, $problem);
}

# Array of global information for output to statistics file.
#
# Each element is hash with fields
#   - title
#   - content
my @test_statistics = ();

# Add tests statistic for output.
#
# Arguments:
#
# $1 - title.
# $2 - content.
sub add_test_statistic($$)
{
    my ($title, $content) = @_;
    
    my $test_statistic = {
        'title' => $title,
        'content' => $content,
    };
    
    push (@test_statistics, $test_statistic);
}


#---------------------------------------------------------------------
# The problem database, hashed by the "test_name test_point" strings.
# Each element is array of problem records, corresponded to test(s) with
# given identificators.
my %problem_database;

# Create empty problem record
sub problem_record_new()
{
    my $problem_record = {
        'expected' => [],
        'versions' => [],
        'matched' => [],
        'attributes' => {},
    };
    
    return $problem_record;
}

# Auxiliary function, add problem record into hash map
#
# Arguments:
#
# $1 - problem_record(hash)
# $2 - test_name
# $3 - test_point
#
sub add_problem_record($$$)
{
    my ($problem_record, $test_name, $test_point) = @_;
    
    my $id = $test_name . " " . $test_point;
    
    my $problem_array = $problem_database{$id};
    if(not defined $problem_array)
    {
        $problem_array = [];
        $problem_database{$id} = $problem_array;
    }
    
    push(@$problem_array, $problem_record);
}

# Auxiliary function, add 'expected' check for severity property.
#
# Arguments:
#
# $1 - problem record
# $2 - expected severity
sub problem_record_set_expected_severity($$)
{
    my ($problem_record, $expected_severity) = @_;
    
    $problem_record->{'expected_severity'} = $expected_severity;
}

# Auxiliary function, add 'expected' check for property.
#
# Arguments:
#
# $1 - problem record
# $2 - property name
# $3 - expected value
sub problem_record_add_expected_property($$$)
{
    my ($problem_record, $property_name, $expected_value) = @_;
    
    if($property_name eq 'severity')
    {
        problem_record_set_expected_severity($problem_record, $expected_value);
        return;
    }
    
    my $expected_desc = {
        'property_name' => $property_name,
        'expected_value' => $expected_value
    };
    
    my $expected_descs = $problem_record->{'expected'};
    
    push(@$expected_descs, $expected_desc);
}


# Auxiliary function, add 'matched' check for property.
#
# Arguments:
#
# $1 - problem record
# $2 - property name
# $3 - matched value
sub problem_record_add_matched_property($$$)
{
    my ($problem_record, $property_name, $matched_value) = @_;
    
    my $matched_desc = {
        'property_name' => $property_name,
        'matched_value' => $matched_value
    };
    
    my $matched_descs = $problem_record->{'matched'};
    
    push(@$matched_descs, $matched_desc);
}

# Auxiliary function, add 'version' check for property.
#
# Arguments:
#
# $1 - problem
# $2 - property name
# $3 - compared value
# $4 - compared mask
#
# Mask(last argument) should be hash, in which next keys may be tested:
#   - "-1" - whether 'less' is accepted,
#   - "0" - whether 'equal' is accepted,
#   - "1" - whether 'greater' is accepted.
sub problem_record_add_version_property($$$$)
{
    my ($problem_record, $property_name, $compared_value, $compared_mask) = @_;
    
    my $version_desc = {
        'property_name' => $property_name,
        'compared_value' => $compared_value,
        'compared_mask' => $compared_mask,
    };
    
    my $version_descs = $problem_record->{'versions'};
    
    push(@$version_descs, $version_desc);
}


# Auxiliary function, add value for set attribute.
#
# Arguments:
#
# $1 - problem
# $2 - attribute name
# $3 - value
sub problem_record_add_attribute($$$)
{
    my ($problem_record, $attribute_name, $value) = @_;
    
    my $attributes = $problem_record->{'attributes'};
    
    my $current_value = $attributes->{$attribute_name};
    
    if(not defined $current_value)
    {
        $attributes->{$attribute_name} = $value;
    }
    else
    {
        $attributes->{$attribute_name} = $current_value . '\n' . $value;
    }
}


# Loads the problem database which is used for automatic identification
# of known test suite failures, warnings and false positives.
#
# Arguments:
#
# $1 - name of file, contained problem database.
#
# Problem database format description:
#
# 1.    Empty(contain only space charactes) lines and lines started
#       (possibly, after spaces) with '#' are igonred.
#
# 2.    Other lines should have format
#           {keyword}[...] 
#       Keyword is case insensetive.
#
# 3.    Problem definition boundaries:
#       - test {testname}[ {testpoint}]
#           line starts problem definition for test, characterized
#           by testname and testpoint.
#       - end
#           line ends problem definition
#
# 4.    Problem matchers check value of test property. It may be
#       property of the single test, property of all tests in the
#       journal, or property of the system under test.
#       Problem definition is applied to the problem only if all checks
#       are true.
#       - expected {property} {value}
#           checks that property has given value.
#       - matched {property} {substring}
#           checks that property contain given substring.
#       - version l|le|g|ge {property} {value}
#           checks that property is stringify less | less or equal |
#           greater | greater or equal. This check usually performed for
#           properties which means version of something.
#       At least one 'expected' expression should check 'severity' test
#       property.
#
# 5.    Test attribute assignment create new attribute for the test.
#       This attribute may be used for output problem information to the
#       user.
#       - attribute {name} {value}
#           assign value to attribute with given name. If same name is
#           specified in several 'attribute' directives, then all values
#           will be joined together with newline symbol between.
#
# 6.    Share problem definition for several tests.
#       - dup {testname}[ {testpoint}]
#           share current problem definition with the test, characterized
#           by testname and testpoint.
#       

sub load_problem_database {
    my $file_name = shift;

    # Open the database file.
    unless ( open(FILE, $file_name) ) {
        print "Could not open problem database $file_name\n";
        return;
    }

    my $line_number = 0;
    my $line;

    my $parse_error = sub
    {
        my $message = shift;
        fail("problem database: $line_number: ${message}");
    };

    # The problem record being read.
    my $problem_record = undef; # 'undef' means that problem is not created.
    
    # Patterns for match some essences.
    my $re_name = qr/[\w_][\w\d_]*/;
    my $re_value = qr/.*/;
    my $re_test_name = qr/\S+/;
    my $re_test_point = qr/\S+/;

    while ($line = <FILE>) {

        # Increment the line number.
        $line_number++;

        # Skip empty lines.
        if($line =~ /^\s*$/) {
            next;
        }
        # Skip comment lines.
        elsif($line =~ /^\s*#/) {
            next;
        }
        # Only 'test' directive is allowed to start problem record.
        elsif(not defined $problem_record)
        {
            # The 'test' line starts a new test record.
            # It contains the name of the test and the test point number.
            if($line =~ /^test\s+(${re_test_name})\s+(${re_test_point})$/)
            {
                my $test_name = $1;
                my $test_point = $2;

                # Create new problem record.
                $problem_record = problem_record_new();
                
                add_problem_record($problem_record, $test_name, $test_point);
            }
            # Different errors
            elsif($line =~ /^test(\s+.*)?$/) {
                &$parse_error("Incorrect format of 'test' directive");
            }
            elsif($line =~ /^([\w_][\w\d_]*)\s*(.*)$/) {
                &$parse_error("Unexpected directive '$1' while 'test' expected.");
            }
            else {
                &$parse_error("Syntax error");
            }
        }
        # Other directives are allowed only when problem record
        # definition is started.
        elsif($line =~ /^end(\s+.*)?/) {
            # Check if this problem report has all required fields.
            until (defined $problem_record->{'expected_severity'}) {
                # Expected severity must be provided.
                &$parse_error("Terminated record has no expected severity");
            }
            # Clear problem reference.
            $problem_record = undef;
        }
        elsif($line =~ /^expected\s+(${re_name})\s*(${re_value})$/) {
            my $name = $1;
            my $value = $2;
            
            problem_record_add_expected_property($problem_record, $name, $value);
        }
        elsif ($line =~ /^matched\s+(${re_name})\s*(${re_value})$/) {
            my $name = $1;
            my $value = $2;

            problem_record_add_matched_property($problem_record, $name, $value);
        }

        elsif ($line =~ /^version\s+(l|le|g|ge)\s+(${re_name})\s*(${re_value})$/) {
            my $type = $1;
            my $name = $2;
            my $value = $3;
            
            my $mask = {};
            if($type eq "l")
            {
                $mask->{"-1"} = "1";
            }
            elsif($type eq "le")
            {
                $mask->{"-1"} = "1";
                $mask->{"0"} = "1";
            }
            elsif($type eq "g")
            {
                $mask->{"1"} = "1";
            }
            elsif($type eq "ge")
            {
                $mask->{"1"} = "1";
                $mask->{"0"} = "1";
            }
            else
            {
                die("Unreachable");
            }
            
            problem_record_add_version_property($problem_record, $name, $value, $mask);
        }
        elsif ($line =~ /^attribute\s+(${re_name})(\s+(${re_value}))?$/) {
            my $name = $1;
            my $value = "";
            
            if(defined $2)
            {
                $value = $3;
            }

            problem_record_add_attribute($problem_record, $name, $value);
        }

        elsif ($line =~ /^dup\s+(${re_test_name})\s+(${re_test_point})$/) {
            my $test_name = $1;
            my $test_point = $2;

            add_problem_record($problem_record, $test_name, $test_point);
        }
        # Anything else is a syntax error.
        else {
            #TODO: more precise error decription
            &$parse_error("Syntax error.");
        }
    }
    close FILE;
}

#---------------------------------------------------------------------
# Array of journals
my @journals = ();

# Create journal and add it to the list of journals.
#
# Arguments:
#
# $1 - title
# $2 - anchor
#
# Return: added journal(hash reference).
sub add_journal($$)
{
    my ($journal_title, $journal_anchor) = @_;
    
    my $journal = {
        'journal_title' => $journal_title,
        'journal_anchor' => $journal_anchor,
        
        'stats_tests_total'             => 0,
        'stats_tests_passed'            => 0,
        'stats_tests_skipped'           => 0,
        'stats_tests_quelled'           => 0,
        'stats_tests_failed'            => 0,
        'stats_tests_failed_known'      => 0,
        
        'problems' => [],
        'infos' => [],
    };
    
    push (@journals, $journal);
    
    return $journal;
}

# Set group for the journal
#
# Arguments:
#
# $1 - journal
# $2 - name of the group
#
sub journal_set_group($$)
{
    my ($journal, $group_name) = @_;
    
    $journal->{'group'} = $group_name;
}

# Set subgroup for the journal
#
# Arguments:
#
# $1 - journal
# $2 - name of the subgroup
#
sub journal_set_subgroup($$)
{
    my ($journal, $subgroup_name) = @_;
    
    $journal->{'subgroup'} = $subgroup_name;
}


#---------------------------------------------------------------------
# Prints time and date in a human-readable format
sub format_time {
    my $timestamp = shift;
    
    my ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday) = 
        localtime ($timestamp);

    return sprintf ("%02d:%02d:%02d %02d-%02d-%04d\n", $hour, $min, $sec,
        $mday, ($mon + 1), ($year + 1900) );
}

#---------------------------------------------------------------------

#---------------------------------------------------------------------
# 'Test' is a reference to hash, contained:
#   - journal reference
#   - test_name
#   - test_point
#   - [test_number] (not used now)
#   - severity
#   - test_messages
# (according to problem db)
#   - quelled
#   - waived
#   - resolution
#   - comment
#   - lsb_report_id
#   - lsb_resolution_id
#

# Create new test object for given journal
#
# Arguments:
#
# $1 - journal
#
# Return test object created.
sub new_test($)
{
    my $journal = shift;
    
    my $test = {
        'journal' => $journal
    };
    
    return $test;
}

#test_set_name $test $test_name
sub test_set_name($$)
{
    my ($test, $test_name) = @_;
    
    $test->{'test_name'} = $test_name;
}

#test_set_point $test $test_point
sub test_set_point($$)
{
    my ($test, $test_point) = @_;
    
    $test->{'test_point'} = $test_point;
}

#test_set_number $test $test_number
sub test_set_number($$)
{
    my ($test, $test_number) = @_;
    
    $test->{'test_number'} = $test_number;
}

#test_set_severity $test $severity
sub test_set_severity($$)
{
    my ($test, $severity) = @_;
    
    $test->{'severity'} = $severity;
}

#test_set_messages $test $test_messages
sub test_set_messages($$)
{
    my ($test, $test_messages) = @_;
    
    $test->{'test_messages'} = $test_messages;
}


#---------------------------------------------------------------------
# Auxiliary function, extract given property of the test.
#
# Arguments:
#
# $1 - test
# $2 - property name
sub test_get_property($$)
{
    my ($test, $property_name) = @_;
    
    my $value = undef;
    
    $value = $test->{$property_name};
    return $value if(defined $value);
    
    my $journal = $test->{'journal'};
    $value = $journal->{$property_name};
    return $value if(defined $value);
    
    $value = $system_info{$property_name};
    return $value if(defined $value);
    
    return undef;
}

# Auxiliary function, check, that given problem record may be applied
# to the test.
#
# NOTE: test_name and test_point should be checked before call this function.
#
# Arguments:
#
# $1 - problem
# $2 - test
#
#
sub problem_record_is_applicable($$)
{
    my ($problem_record, $test) = @_;
    
    # Check that severity is expected.
    my $severity = $test->{'severity'};
    my $expected_severity = $problem_record->{'expected_severity'};
    if (lc($severity) ne lc($expected_severity))
    {
        return 0;
    }

    # Check that other properties are expected
    for my $expected_desc (@{$problem_record->{'expected'}})
    {
        my $property_name = $expected_desc->{'property_name'};
        my $expected_value = $expected_desc->{'expected_value'};
       
        my $value = test_get_property($test, $property_name);

        if(not defined $value or ($value ne $expected_value))
        {
            return 0;
        }
    }
    
    # Check versions
    for my $version_desc (@{$problem_record->{'versions'}})
    {
        my $property_name = $version_desc->{'property_name'};
        my $compared_value = $version_desc->{'compared_value'};
        my $compared_mask = $version_desc->{'compared_mask'};
       
        my $value = test_get_property($test, $property_name);

        return 0 until(defined $value);
        
        return 0 until $compared_mask->{$value cmp $compared_value};
    }

    # After all, check matches
    for my $matched_desc (@{$problem_record->{'matched'}})
    {
        my $property_name = $matched_desc->{'property_name'};
        my $matched_value = $matched_desc->{'matched_value'};

        my $value = test_get_property($test, $property_name);
        
        if(not defined $value or (index($value, $matched_value) == -1))
        {
            return 0;
        }
    }

    return 1;
}

# Finds a problem record in the database.
#
# Arguments:
# $1 - test descriptor
sub find_problem_record
{
    my $test = shift;
    
    # The basic record key is the name of the test and the test point number.
    my $record_key = $test->{'test_name'};
    if(defined $test->{'test_point'})
    {
        $record_key = $record_key . " " . $test->{'test_point'};
    }
    
    my $problem_array = $problem_database{$record_key};
    
    return unless (defined $problem_array);
    
    for my $problem_record (@$problem_array)
    {
        return $problem_record if(problem_record_is_applicable($problem_record, $test));
    }

    # Nothing was found.
    return undef;
}

#---------------------------------------------------------------------
# Problem unique identificator to be assigned next.
my $problem_id_next = "1";

# Add test problem description to the journal.
#
# Arguments:
#
# $1 - journal.
# $2 - problem.
sub journal_add_problem_test
{
    my ($journal, $problem) = @_;

    my $problems = $journal->{'problems'};
    
    if(not defined $problems)
    {
        $problems = [];
        $journal->{'problems'} = $problems;
    }
    
    push (@$problems, $problem);
}

#---------------------------------------------------------------------
# Process the result of a test point.
#
# Arguments:
#
# $1 - journal
# $2 - test
sub process_test_point_result {
    my ($journal, $test) = @_;

    # Sanity check: Test name must be valid.
    unless ($test->{'test_name'}) {
        print ("Empty test name\n");
        return;
    }

    # Compare severity strings in lowercase.
    my $lowercase_severity = lc($test->{'severity'});

    # Increment the total test count.
    $journal->{stats_tests_total} += 1;

    # Passed and unused tests are not printed, only counted.
    if ($lowercase_severity eq "passed") {
        $journal->{stats_tests_passed} += 1;
        return;
    }
    elsif (($lowercase_severity eq "unused") || ($lowercase_severity eq "untested") ||
        ($lowercase_severity eq "unsupported") || ($lowercase_severity eq "unimplemented") ||
        ($lowercase_severity eq "skipped")) {
        $journal->{stats_tests_skipped} += 1;
        return;
    }

    elsif (($lowercase_severity eq "failed") || ($lowercase_severity eq "failure") ||
        ($lowercase_severity eq "unresolved") || ($lowercase_severity eq "unreported") ||
        ($lowercase_severity eq "uninitiated") || ($lowercase_severity eq "test suite error") || 
        ($lowercase_severity eq "time expired")) {

        # Process this case outside of 'if' clause.
    }
    
    else {
        fail("Unrecognized severity code '$test->{'severity'}'\n");
    }
    
    # Test is failed.
    # But before increment counter, need to determine type of failure.

    # Create problem object for it.
    my $problem = {
        problem_id => $problem_id_next,
        
        test_name => $test->{'test_name'},
        test_point => $test->{'test_point'},
        test_number => $test->{'test_number'},
        
        test_messages => $test->{'test_messages'},
        
        severity => $test->{'severity'},
    };
    
    $problem_id_next++;
    
    # Search problem description in the database, which fit for the test.
    my $problem_record = find_problem_record($test);
    if (defined $problem_record) {
        
        # Assign all attributes from problem record to problem.
        my $attributes = $problem_record->{'attributes'};
        
        for my $attribute_name (keys %$attributes)
        {
            $problem->{$attribute_name} = $attributes->{$attribute_name};
        }
        # Force 'quell' attribute for all known problems if correponded
        # option is given to program.
        if($known_are_quelled)
        {
            $problem->{'quell'} = "";
        }
        
        if (defined $problem->{'quell'}) {
            # If quelled, add quelled suffix to the test severity.
            # This means it can be safely ignored.
            $problem->{'severity'} .= " (quelled)";
        }
        else
        {
            # Otherwise, mark problem as known failure
            $problem->{'known_failure'} = 1;
        }
        
    }
    # Now update counters and set severity flag.
    if(defined $problem->{'quell'}) {
        $journal->{'stats_tests_quelled'} += 1;
        $problem->{'severity_harmless'} = "true";
    }
    else {
        $journal->{'stats_tests_failed'} += 1;
        $problem->{'severity_failed'} = "true";
        if(defined $problem->{'known_failure'}) {
            $journal->{'stats_tests_failed_known'} += 1;
        }
    }
    
    journal_add_problem_test($journal, $problem);
}

#---------------------------------------------------------------------

# Add information about tests for output.
#
# Arguments
#
# $1 - journal
# $2 - title.
# $3 - content.
# $4 - significance (info, neutral, success, failed, warning), default is "info"

sub journal_add_info
{
    my ($journal, $title, $content, $significance) = @_;
    
    my $info = {'title' => $title, 'content' => $content};
    
    if(not $significance)
    {
        $significance = "info";
    }
    
    $info->{"significance_$significance"} = "true";
    
    my $infos = $journal->{'infos'};
    
    push (@$infos, $info);
}


# Add statistic information about tests for output.
#
# Arguments
#
# $1 - journal
# $2 - title.
# $3 - content.
# $4 - significance (info, neutral, success, failed, warning), default is "neutral"

sub journal_add_statistics
{
    my ($journal, $title, $content, $significance) = @_;
    
    if(not $significance)
    {
        $significance = "neutral";
    }
    
    journal_add_info($journal, $title, $content, $significance);
}

#---------------------------------------------------------------------

# Form information for journal.
#
# Arguments
#
# $1 - journal

sub journal_create_statistic
{
    my $journal = shift;

    # Calculate the time interval between the tests start and stop.
    my $test_duration = "";

    if (defined($journal->{'start_time'}))
    {
        my $start_time = $journal->{'start_time'};
        if (defined($journal->{'stop_time'}))
        {
            my $stop_time = $journal->{'stop_time'};
            
            $test_duration = $stop_time - $start_time;
            my $test_duration_sec = $test_duration % 60;
            $test_duration = int($test_duration / 60);
            my $test_duration_min = $test_duration % 60;
            my $test_duration_hour = int($test_duration / 60);
            
            if($test_duration_hour > 0) {
                $test_duration = "${test_duration_hour}h:${test_duration_min}m:${test_duration_sec}s";
            }
            else {
                $test_duration = "${test_duration_min}m:${test_duration_sec}s";
            }
            journal_add_statistics($journal, "Test time", $test_duration);
        }
        else
        {
            journal_add_statistics($journal, "Start time", format_time($start_time));
        }
    }
    elsif (defined($journal->{'stop_time'}))
    {
        my $stop_time = $journal->{'stop_time'};
        journal_add_statistics($journal, "Stop time", format_time($stop_time));
    }

    
    journal_add_statistics($journal, "Total tests", $journal->{'stats_tests_total'});
    
    journal_add_statistics($journal, "Passed tests", $journal->{'stats_tests_passed'});

    if ($journal->{'stats_tests_skipped'} > 0) {
        journal_add_statistics($journal, "Tests not relevant/not applicable", $journal->{'stats_tests_skipped'});
    }

    if ($journal->{'stats_tests_quelled'} > 0) {
        journal_add_statistics($journal, "Quelled problems", $journal->{'stats_tests_quelled'});
    }

    if ($journal->{'stats_tests_failed'} > 0) {
        journal_add_statistics($journal, "Failures", $journal->{'stats_tests_failed'}, "failed");
    }
    
    if ($journal->{'stats_tests_failed_known'} > 0) {
        journal_add_statistics($journal, "Failures(known)", $journal->{'stats_tests_failed_known'}, "failed");
    }
    
    my $stats_tests_failed_new = $journal->{'stats_tests_failed'} - $journal->{'stats_tests_failed_known'};

    if ($stats_tests_failed_new > 0) {
        journal_add_statistics($journal, "Failures(new)", $stats_tests_failed_new, "failed");
    }

    # Give the final verdict.
    my $final_verdict;
    if($journal->{'status_failed'})
    {
        $final_verdict = "Failure";
    }
    elsif($journal->{'status_warning'})
    {
        $final_verdict = "Warnings";
    }
    elsif($journal->{'status_success'})
    {
        $final_verdict = "Success";
    }
    else
    {
        fail("Unknown status of journal");
    }
    journal_add_info($journal, "Verdict", $final_verdict);
}

# Process information in filled journal
#
# Arguments:
#
# $1 - journal
sub journal_postprocess
{
    my $journal = shift;
    # set final verdict
    if($journal->{'stats_tests_failed'} > 0)
    {
        $journal->{'status'} = "Failures: " . $journal->{'stats_tests_failed'};
        $journal->{'status_failed'} = "1";
    }
    else
    {
        $journal->{'status_success'} = "1";
        $journal->{'status'} = "Success";
    }
}

#---------------------------------------------------------------------

# Parse fake journals - for test scripts.
#
# Arguments:
#
# $1 - directory with journals
sub parse_fake_journals
{
    #my $dir = shift;
    
    my $journal = add_journal("test_journal", "test_journal");
    
    $journal->{'group'} = "Group1";
    $journal->{'subgroup'} ="Subgroup1";
    
    my $test1 = new_test($journal);
    
    $test1->{'test_name'} = "Test1";
    $test1->{'severity'} = "Passed";
    $test1->{'test_messages'} = "Some message from test.";
    
    process_test_point_result($journal, $test1);

    my $test2 = new_test($journal);
    
    $test1->{'test_name'} = "Test2";
    $test1->{'severity'} = "Failed";
    $test1->{'test_messages'} = "Error: this error is fake.";
    
    process_test_point_result($journal, $test2);
}

#---------------------------------------------------------------------
# Auxiliary functions for extract information from parsed XML.
#
# xml_get_name $elem
sub xml_get_name($)
{
    my $elem = shift;
    return ${$elem}[0];
}

#xml_get_children $elem
sub xml_get_children($)
{
    my $elem = shift;
    my $n_children = $#{$$elem[1]} / 2;
    
    my @children = ();
    for(my $i = 0; $i < $n_children; $i++)
    {
        push(@children, [$$elem[1][$i * 2 + 1], $$elem[1][$i * 2 + 2]]);
    }
    
    return @children;
}

# xml_get_first_subelem elem, child_name
sub xml_get_first_child($$)
{
    my ($elem, $child_name) = @_;

    foreach my $child (xml_get_children($elem))
    {
        return $child if (xml_get_name($child) eq $child_name);
    }

    return undef;
}


# xml_get_attr elem attr_name
sub xml_get_attr($$)
{
    my ($elem, $attr_name) = @_;
    
    my $attributes = $$elem[1][0];
    
    my $attr = $attributes->{$attr_name};
    
    return $attr;
}

#xml_get_text elem
sub xml_get_text($)
{
    my $elem = shift;
    my $text = "";
    foreach my $child (xml_get_children($elem))
    {
        return undef unless(xml_get_name($child) eq "0");
        $text = $text.$$child[1];
    }

    return $text;
}

#---------------------------------------------------------------------
# Parse Spruce index file.
#
# Arguments:
#
# $1 - filename
sub spruce_parse_index_file($)
{
    my $filename = shift;

    my $index_parser = new XML::Parser("Style"=>"Tree");
    my $index_struct = $index_parser->parsefile($filename);

    unless ($index_struct)
    {
        fail("Format of index file '$filename' is not XML.");
    }
    
    my $spruce_index_parse_error = sub {
        my $message = shift;
        fail("Incorrect format of Spruce index file '$filename':\n" . $message);
    };
    
    my $root_name = xml_get_name($index_struct);
    unless($root_name eq "Main")
    {
        &$spruce_index_parse_error("Root element has name '$root_name', but should have name 'Main'.");
    }

    # Extract text enclosed in tags
    #
    # Arguments:
    #
    # $1 - tag_name
    my $extract_tagged_text = sub {
        my $tag_name = shift;
        my $tag_elem = xml_get_first_child($index_struct, $tag_name);
		unless($tag_elem)
		{
			&$spruce_index_parse_error("XML element \"$tag_name\" is missed.");
		}
		
		my $text = xml_get_text($tag_elem);
		unless(defined $text)
		{
			&$spruce_index_parse_error("XML element \"$tag_name\" contains something other than text.")
		}
		chomp $text;
        
        return $text;
    };

    # Extract architecture of system under test.
    my $arch = &$extract_tagged_text("Arch");
    add_test_info("Architecture", $arch);

    # Extract distibution.
    my $distro = &$extract_tagged_text("Distribution");
	# Extract distibution version.
	my $distroVersion = &$extract_tagged_text("DistroVersion");
    add_test_info("Distribution", $distro . " " . $distroVersion);

	# Extract kernel version
	my $kernel_version = &$extract_tagged_text("Kernel");
	add_test_info("Kernel", $kernel_version);

    # Use "x86" for 32-bit x86-like architectures.
    # TODO: determine other architectures.
    if($arch =~ /64/)
    {
		$system_info{'arch'} = "x86_64";
	}
	else
	{
		$system_info{'arch'} = "x86";
	}
    
    $system_info{'distro'} = $distro;
    $system_info{'distro_version'} = $distroVersion;

    # For suite for string comparision cut everything after first number in kernel version
    my $kernel_version_simple = $kernel_version;
    $kernel_version_simple =~ s/^(\d+)\.(\d+)\.(\d+)*$/$1.$2.$3/;
    $system_info{'kernel_version'} = $kernel_version_simple;
}

#---------------------------------------------------------------------

sub spruce_report_parse_error($$)
{
    my ($filename, $comment) = @_;
    
    add_problem_other($filename, "Parse error", $comment);
}

# Map of some Spruce test statuses into severity strings.
my %spruce_status_severity_map = (
    "Fatal"    => "Unresolved",
    "Shallow"  => "Passed",
    "Signaled" => "Unresolved",
    "Success"  => "Passed",
    "Timeout"  => "Time expired",
    "Unknown"  => "Unresolved",
);

# Parse one Spruce log file
#
# Arguments:
#
# $1 - name of log file.
# $2 - XML parser.
sub spruce_parse_log_file($$)
{
    my ($filename, $xml_parser) = @_;
    # Filename without directory and extension parts. Used for some purposes.
    my $filename_base = $filename;
    $filename_base =~ s/.*\///;# Remove directory part
    $filename_base =~ s/\..*$//;# Remove extension

    
    # Auxiliary function for report error while parsing spruce journal.
    #
    # Arguments:
    #
    # $1 - comment
    my $spruce_report_parse_error = sub {
        my $comment = shift;
        add_problem_other($filename, "Parse error", $comment);
    };
    
    my $log = $xml_parser->parsefile($filename);
    unless($log)
    {
        &$spruce_report_parse_error("Format of log file is not XML.");
        return;
    }
    
    my $root_name = xml_get_name($log);
    unless($root_name eq "SpruceLog")
    {
        &$spruce_report_parse_error( 
            "Root element has name '$root_name', but should have name 'SpruceLog'.");
        return;
    }   

    my $fs_elem = xml_get_first_child($log, "FS");
    unless($fs_elem)
    {
        &$spruce_report_parse_error("XML element \"FS\" is missed.");
        return;
    }
    
    my $fs = xml_get_attr($fs_elem, "Name");
    unless($fs)
    {
        &$spruce_report_parse_error("Attribute \"Name\" of XML element \"FS\" is missed.");
        return;
    }
    
    my $mount_opts = xml_get_attr($fs_elem, "MountOptions");
    unless(defined $mount_opts)
    {
        &$spruce_report_parse_error("Attribute \"MountOptions\" of XML element \"FS\" is missed.");
        return;
    }
    
    my $module_elem = xml_get_first_child($fs_elem, "Module");
    unless($module_elem)
    {
        &$spruce_report_parse_error("XML element \"Module\" is missed.");
        return;
    }

    my $test_suite = xml_get_attr($module_elem, "Name");
    unless($test_suite)
    {
        &$spruce_report_parse_error("Attribute \"Name\" of XML element \"Module\" is missed.");
        return;
    }
    
    # All journal attributes are known now, create it.
    my $journal_title = $fs . "." . $test_suite;
    if($mount_opts)
    {
        $journal_title .= "." . $mount_opts;
    }

    # Currently, crosscompilation flag (32bit on 64 bit) is not stored directly.
    # Instead, testsuite name has corresponded suffix.
    #
    # Cut testsuite bit suffix and set crosscompilation flag if needed.
    my $test_32_on_64 = 0;
    if($test_suite =~ /_(32|64)$/)
    {
        if($1 eq "32")
        {
            $test_32_on_64 = 1;
        }

        $test_suite =~ s/_(32|64)$//;
        
        # NB: suffix is stored in journal name, which is shown to the user.
    }
   
    my $journal = add_journal($journal_title, $filename_base);
    $journal->{'group'} = $fs;
    $journal->{'subgroup'} = $test_suite;

    # Setup properties of journal(common for all tests in it).
    $journal->{'fs'} = $fs;
    $journal->{'mount_opts'} = $mount_opts;
    $journal->{'test_suite'} = $test_suite;
    
    if($test_32_on_64)
    {
        $journal->{'code_bits'} = 32;
    }
    else
    {
        $journal->{'code_bits'} = 64;
    }
    
    # Add tests to the journal.
    foreach my $item_elem (xml_get_children($module_elem))
    {
        next unless (xml_get_name($item_elem) eq "Item");
        
        my $test_name = xml_get_attr($item_elem, "Name");
        unless(defined $test_name)
        {
            &$spruce_report_parse_error("Attribute \"Name\" of XML element \"Item\" is missed.");
            return;
        }
        
        # Test point is empty by default
        my $test_point = "";
        my $operation_elem = xml_get_first_child($item_elem, "Operation");
		if($operation_elem)
		{
            $test_point = xml_get_text($operation_elem);
            unless(defined $test_point)
            {
                &$spruce_report_parse_error("XML element \"Operation\" contains something other than text.");
                return;
            }
		}

        my $status_elem = xml_get_first_child($item_elem, "Status");
        unless($status_elem)
        {
            &$spruce_report_parse_error("XML element \"Status\" is missed in \"Item\" element.");
            return;
        }

        my $status = xml_get_text($status_elem);
        unless(defined $status)
        {
            &$spruce_report_parse_error("XML element \"Status\" contains something other than text.");
            return;
        }
        
        # Convert status to severity.
        my $severity = $spruce_status_severity_map{$status}
            ? $spruce_status_severity_map{$status}
            : $status;

        my $output_elem = xml_get_first_child($item_elem, "Output");
        unless($output_elem)
        {
            &$spruce_report_parse_error("XML element \"Output\" is missed in \"Item\" element.");
            return;
        }

        my $test_messages = xml_get_text($output_elem);
        unless(defined $test_messages)
        {
            &$spruce_report_parse_error("XML element \"Output\" contains something other than text.");
            return;
        }
        
        my $test = new_test($journal);

        $test->{'test_name'} = $test_name;
        $test->{'test_point'} = $test_point;
        $test->{'test_messages'} = $test_messages;

        $test->{'severity'} = $severity;
        
        process_test_point_result($journal, $test);
    }
}
# Parse Spruce journals.
#
# Arguments:
#
# $1 - directory with journals
sub parse_spruce_journals
{
    my $dir = shift;
    
    $test_system = "Spruce";
    
    my $index_filename = "$dir/main.xml";
    
    if(! -e $index_filename)
    {
		fail("Spruce tests index file '$index_filename' is absent");
	}

    spruce_parse_index_file($index_filename);

    my $log_parser = new XML::Parser("Style"=>"Tree");
    
    opendir(DIR, $dir) or die $!;

    while (my $file = readdir(DIR)) {

        # We only want files
        next unless (-f "$dir/$file");

        # Use a regular expression to find files ending in _log.xml
        next unless ($file =~ m/_log\.xml$/);

        spruce_parse_log_file("$dir/$file", $log_parser);
    }
    
    closedir(DIR);
    
    @journals = sort {$a->{'journal_title'} cmp $b->{'journal_title'}} @journals;
}


# Sum values from all journals. Usefull for compute overall statistics.
#
# Arguments:
#
# $1 - field name
sub sum_for_journals($)
{
    my $field_name = shift;
    
    my $result = 0;
    
    for my $journal (@journals)
    {
        $result += $journal->{$field_name};
    }
    
    return $result;
}

# Parse all journals

sub parse_journals
{
    my $journals_dir = shift;
    
    parse_spruce_journals($journals_dir);
    
    # Postprocess of journals.
    
    for my $journal (@journals)
    {
        journal_postprocess($journal);
        journal_create_statistic($journal);
    }
    
    # Compute overall statistic.
    
    $tests_total = sum_for_journals('stats_tests_total');
    $tests_passed = sum_for_journals('stats_tests_passed');
    $tests_skipped = sum_for_journals('stats_tests_skipped');
    $tests_quelled = sum_for_journals('stats_tests_quelled');
    $tests_failed = sum_for_journals('stats_tests_failed');
    $tests_failed_known = sum_for_journals('stats_tests_failed_known');
    
    add_test_statistic("Total", $tests_total);
    
    if($tests_passed > 0)
    {
        add_test_statistic("Passed", $tests_passed);
    }
    
    if($tests_skipped > 0)
    {
        add_test_statistic("Skipped", $tests_skipped);
    }
    
    if($tests_quelled > 0)
    {
        add_test_statistic("Quelled", $tests_quelled);
    }

    if($tests_failed > 0)
    {
        add_test_statistic("Failed", $tests_failed);
    }
    
    if($tests_failed_known > 0)
    {
        add_test_statistic("Failed(known)", $tests_failed_known);
    }

    my $tests_failed_new = $tests_failed - $tests_failed_known;
    
    if($tests_failed_new > 0)
    {
        add_test_statistic("Failed(new)", $tests_failed_new);
    }

}

#---------------------------------------------------------------------

# Create 'index' for report.
#
# Arguments:
#
# $1 - name of the file created.
sub compile_index
{
    my $filename = shift;
    
    my $file;
    unless (open($file, "> $filename")) {
        print "Couldn't open file '$filename' for writing.\n";
        return;
    }

    # Create topgroup/subgroup divisions
    
    my %top_groups_map = ();# Map of subgroups references.
    
    for my $journal (@journals)
    {
        my $top_group_name = $journal->{'group'};
        if(not defined $top_group_name)
        {
            $top_group_name = "";
        }
        
        # Reference to map of journals arrays references.
        my $subgroups_map = $top_groups_map{$top_group_name};
        
        if(not defined $subgroups_map)
        {
            $subgroups_map = {};
            $top_groups_map{$top_group_name} = $subgroups_map;
        }
        
        my $subgroup_name = $journal->{'subgroup'};

        if(not defined $subgroup_name)
        {
            $subgroup_name = "";
        }
        
        # Reference to journals array.
        my $subgroup = $subgroups_map->{$subgroup_name};
        
        if(not defined $subgroup)
        {
            $subgroup = [];
            $subgroups_map->{$subgroup_name} = $subgroup;
        }
        
        push (@$subgroup, $journal);
    }
    
    # Transforms maps into arrays.
    my @top_groups = (); # Array of subgroup references
    
    for my $top_group_name (keys %top_groups_map)
    {
        my $sub_groups = [];
        
        my $top_group = {
            top_group_name => "$top_group_name",
            sub_groups => $sub_groups
        };
        
        my $sub_group_map = $top_groups_map{$top_group_name};
        
        for my $sub_group_name (keys %$sub_group_map)
        {
            my $journals_array = $sub_group_map->{"$sub_group_name"};
            my $sub_group = {
                'sub_group_name' => "$sub_group_name",
                'journals' => $journals_array
            };
            
            push (@$sub_groups, $sub_group);
        }
        
        push (@top_groups, $top_group);
    }
    
    my $templates_dir = "$script_directory/templates";
    
    my $template_index = HTML::Template->new(
        filename => "$templates_dir/index.html",
        path => "$templates_dir",
        option   => 'value',
        global_vars => 'true',
        die_on_bad_params => '0' # Exceeded parameters are allowed
    );
    
    $template_index->param(
        journals => \@journals,
        top_groups => \@top_groups,
        test_system => $test_system,
        system_under_test => $system_under_test,
        test_infos => \@test_infos
    );#TODO: other variables(global).
    
    $template_index->output(print_to => $file);
    
    close($file);
    
    print "Index file $filename is created.\n";
}

# Create 'statistics' for report.
#
# Arguments:
#
# $1 - name of the file created.
sub generate_statistics($) {
    my $filename = shift;

    my $file;
    unless (open($file, "> $filename")) {
        fail("Couldn't open file '$filename' for writing.\n");
    }
    
    for my $record (@test_statistics)
    {
        printf ($file "%s: %s\n", $record->{'title'}, $record->{'content'});
    }
    close($file);

    print "Statistics file $filename is created.\n";
}


# Create 'other_problems' for report.
#
# Arguments:
#
# $1 - name of the file created.
sub generate_other_problems($) {
    my $filename = shift;

    my $file;
    unless (open($file, "> $filename")) {
        fail("Couldn't open file '$filename' for writing.\n");
    }
    
    for my $problem (@other_problems)
    {
        printf ($file "%s in %s: %s\n", $problem->{'severity'}, $problem->{'location'}, $problem->{'comment'});
    }
    close($file);

    print "All problems which doesn't related with tests are stored into '$filename'.\n";
}

sub compile_report
{
    my $result_dir = shift;
    
    mkdir("$result_dir");
    
    if(@other_problems)
    {
        print "There are problems, not concerned with particular tests.\n";
        generate_other_problems("$result_dir/other_problems");
    }
    
    compile_index("$result_dir/index.html");

    generate_statistics("$result_dir/statistics");
    
    #TODO: summary
}

#---------------------------------------------------------------------
sub usage
{
    while(<main::DATA>)
    {
        print $_;
    }
}

sub read_command_line
{
    my $opt_problem_db_filename;
    my $opt_journals_dir;
    my $opt_no_problem_db;
    my $opt_known_are_quelled;
    
    my %options = (
        'problem-db=s' => \$opt_problem_db_filename,
        'journals-dir=s' => \$opt_journals_dir,
        'no-problem-db' => \$opt_no_problem_db,
        'known-are-quelled' => \$opt_known_are_quelled,
    );
    
    unless(GetOptions(%options))
    {
        usage();
        exit 1;
    }
    
    unless(@ARGV)
    {
        print "Error: Directory for store results should be specified.\n";
        usage();
        exit 1;
    }
    
    $result_dir = make_path_absolute($ARGV[0]);

    unless($opt_no_problem_db)
    {
        if($opt_problem_db_filename)
        {
            $problem_db_filename = make_path_absolute($opt_problem_db_filename);
        }
        else
        {
            $problem_db_filename = make_path_absolute("${script_directory}/problem_db");
        }
    }

    if($opt_journals_dir)
    {
        $journals_dir = make_path_absolute($opt_journals_dir);
    }
    else
    {
        $journals_dir = make_path_absolute(".");
    }

    if($opt_known_are_quelled)
    {
        $known_are_quelled = 1;
    }
}

### BEGIN ###

read_command_line();

if(defined $problem_db_filename) {
    print "Load problem database ${problem_db_filename} ...\n";
    load_problem_database($problem_db_filename);
    print "Load problem database ${problem_db_filename} ... done.\n";
}
print "Parse journals in directory ${journals_dir} ...\n";
parse_journals($journals_dir);
print "Parse journals in directory ${journals_dir} ... done.\n";

print ("Generate report\n");
compile_report ($result_dir);

### END ###
__END__
Usage:
    report.pl [OPTIONS] <result_dir>

Generate report for test results. Generated files are stored under <result_dir>.

OPTIONS may be:
    --problem-db <problem-db-file>
        Use problem database from given file. Default is <script-dir>/problem_db.
    --no-problem-db
        Do not use problem database at all.
    --journals-dir <journals-dir>
        Search journals in given directory. Default is current directory
        (from which script is run).
    --known-are-quelled
        Assume all known problems to be quelled. Useful for debug tests.
