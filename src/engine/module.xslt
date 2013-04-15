<?xml version="1.0" encoding="utf-8"?>
<!--    module.xslt
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Authors:
//      	Ruzanna Karakozova <r.karakozova@gmail.com>
//			Vahram Martirosyan <vmartirosyan@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.
-->

<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:exsl="http://exslt.org/common"
		extension-element-prefixes="exsl">

<xsl:output method="text" encoding="utf-8" indent="yes"/>
<xsl:include href="testset.xslt" />

	<xsl:template match="/Module">
bool terminate_process = false;	

#include &lt;Common.hpp>
#include &lt;Test.hpp>
#include &lt;File.hpp>
#include &lt;Directory.hpp>
#include &lt;map>
#include &lt;sys/vfs.h>
#include &lt;XmlSpruceLog.hpp>
#include &lt;vector>
using namespace std;

		<xsl:variable name="ModuleName" select="@Name" />		

char * DeviceName = NULL;
char * MountPoint = NULL;
char * FileSystem = NULL;
map&lt;string, int> FileSystemTypesMap;


		
		<xsl:for-each select="TestSet">
			<xsl:variable name="TestClassName"><xsl:value-of select="$ModuleName" /><xsl:value-of select="@Name" />Tests</xsl:variable>
			<xsl:variable name="TestSetFile"><xsl:value-of select="$XmlFolder"/>/<xsl:value-of select="@Name"/>.xml</xsl:variable>
			<xsl:variable name="SourceFile"><xsl:value-of select="$XmlFolder"/>/<xsl:value-of select="@Name"/>.cpp</xsl:variable>
			<exsl:document href="{$SourceFile}" method="text">
				<xsl:apply-templates select="document($TestSetFile)" >
					<xsl:with-param name="ModuleName" select="$ModuleName"/>
					<xsl:with-param name="TestClassName" select="$TestClassName"/>
				</xsl:apply-templates>
			</exsl:document>
extern TestSet <xsl:value-of select="@Name" />Tests;
		</xsl:for-each>
		
		<!--xsl:for-each select="TestSet">
				using <xsl:value-of select="@Name"/>
		</xsl:for-each-->
		
void ModuleSignalHandler(int signum)
{
	Logger::LogWarn("Received Ctrl+C signal!");
	terminate_process = true;
}
int main(int argc, char ** argv)
{
	struct sigaction sa;
	sa.sa_handler = ModuleSignalHandler;
	if (sigaction(SIGINT, &amp;sa, 0) == -1)
	{	
		cerr &lt;&lt; "Cannot set signal handler. " &lt;&lt;strerror(errno);
		return EXIT_FAILURE;
	}

	if (argc &lt; 2)
	{
		cerr &lt;&lt; "No output file specified. Usage: " &lt;&lt; argv[0] &lt;&lt; " &lt;output_file>" &lt;&lt; endl;
		return EXIT_FAILURE;
	}
	
	if ( getenv("Partition") )
		DeviceName = getenv("Partition");

	if ( getenv("MountAt") )
		MountPoint = getenv("MountAt");
		
	if ( getenv("FileSystem") )
		FileSystem = getenv("FileSystem");
		
	FileSystemTypesMap["ext4"] = 0xEF53; //EXT4_SUPER_MAGIC;
	FileSystemTypesMap["btrfs"] = -1; // does not support?
	FileSystemTypesMap["xfs"] = 0x58465342; //XFS_SUPER_MAGIC;
	FileSystemTypesMap["jfs"] = 0x3153464a; //JFS_SUPER_MAGIC
	
	ModuleLog moduleLog(argv[1]);
	
	try
	{
			bool SkipTestset = false;
		<xsl:for-each select="TestSet">
			if(terminate_process)
				goto terminate_exit;
			{
				<!--xsl:variable name="TestClassName"><xsl:value-of select="$ModuleName" /><xsl:value-of select="@Name" />Tests</xsl:variable>
				extern TestSet <xsl:value-of select="@Name" />Tests;
				SkipTestset = false;
				// Check if there are certain tests to run or be excluded
				if ( argc > 2 )
				{				
					if ( !strcmp(argv[2], "-run") )
					{
						SkipTestset = true;
						// Set the tests to run
						for ( int i = 3; i &lt; argc; ++i )
						{						
							if ( strstr(argv[i], "<xsl:value-of select="@Name" />") != NULL )
							{
								SkipTestset = false;
								string test_name = argv[i] + strlen("<xsl:value-of select="@Name" />") + 1;							
								<xsl:value-of select="$ModuleName" /><xsl:value-of select="@Name" />_tests.RunTest(test_name);
							}
						}					
					}
					if ( !strcmp(argv[2], "-exclude") )
					{
						SkipTestset = false;
						// Set the tests to run
						for ( int i = 3; i &lt; argc; ++i )
						{
							string argvi = argv[i];
							string TestSetNameToExclude = argvi.substr(0, argvi.find('.'));
							
							if ( TestSetNameToExclude != "<xsl:value-of select="@Name" />" )
								continue;
							string test_name = argv[i] + strlen("<xsl:value-of select="@Name" />") + 1;
							<xsl:value-of select="$ModuleName" /><xsl:value-of select="@Name" />_tests.ExcludeTest(test_name);
						}
					}
				}-->
				if ( !SkipTestset )
				{					
					TestSetResult Results(<xsl:value-of select="@Name" />Tests.Run());
					
					vector&lt;TestResult*> items = Results.GetResults();
					vector&lt;TestResult*>::iterator i;
					for( i = items.begin(); i != items.end(); i++)
					{
						cerr &lt;&lt; ("Adding item: " + Results.GetName() + " " + 
							(*i)->GetName() + " " + (*i)->GetDescription() + " " +
							(*i)->StatusToString() + " " + (*i)->GetOutput()) &lt;&lt; endl;
						moduleLog.addItem(Item(Results.GetName(), (*i)->GetName(), (*i)->GetDescription(), (*i)->StatusToString(), (*i)->GetOutput(), ""));
					}
					
				<!--xsl:choose>
					<xsl:when test="@RunTests">
						Results.Merge(<xsl:value-of select="$ModuleName" /><xsl:value-of select="@Name" />_tests.Run<xsl:value-of select="@RunTests"/>Tests());
					</xsl:when>
					<xsl:otherwise>
						Results.Merge(<xsl:value-of select="$ModuleName" /><xsl:value-of select="@Name" />_tests.RunNormalTests());
					</xsl:otherwise>
				</xsl:choose-->
				}
			}
		</xsl:for-each>
					
			//ofstream of(argv[1], ios_base::app);
			//of &lt;&lt; "&lt;Module Name=\"" &lt;&lt; argv[0] &lt;&lt; "\">\n" &lt;&lt; Results.ToXML() &lt;&lt; "&lt;/Module>";
			//of.close();
			
			terminate_exit:	
			
			return Success;// Results.GetStatus();
	}
	catch (Exception ex)
	{
		cerr &lt;&lt; "Exception: " &lt;&lt; ex.GetMessage();
		return EXIT_FAILURE;
	}
}
	</xsl:template>
	

</xsl:stylesheet>
