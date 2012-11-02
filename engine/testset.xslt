<?xml version="1.0" encoding="utf-8"?>
<!--    testset.xslt
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

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" encoding="utf-8" />
	

<xsl:template match="/TestSet">
		<xsl:param name="ModuleName" />
		<xsl:param name="TestClassName" />
	<xsl:value-of select="GlobalHeader"/>
	<xsl:for-each select="Requires">
#include &lt;<xsl:value-of select="." />>
	</xsl:for-each> 

#include &lt;map>
#include &lt;process.hpp>
#include &lt;kedr_integrator.hpp>
using std::map;
	<xsl:variable name="TestSetName" select="@Name" />
	
class <xsl:value-of select="$TestClassName" /> : public Process
{
	typedef map&lt;string, int (<xsl:value-of select="$TestClassName" />::*)(vector&lt;string>)> TestMap;
public:
	<xsl:value-of select="$TestClassName" />():
		_tests_to_run(),
		_tests_to_exclude(),
		_name("<xsl:value-of select="$TestClassName" />"),
		_fsim_enabled(false),
		_fsim_point("kmalloc"),		
		_fsim_expression("0"),		
		_tests(),
		_fsim_tests(),
		_fault_count(0),
		_fsim_info_vec(),
		DirPrefix("<xsl:value-of select="$TestClassName" />_dir_"),
		FilePrefix("<xsl:value-of select="$TestClassName" />_file_")		
		//_testCount(<xsl:value-of select="count(Test)"/>),
		//_fsim_testCount(<xsl:value-of select="count(Test[@FaultSimulationReady='true'])"/>)		
	{	
		Process::EnableAlarm = true;
	<xsl:value-of select="StartUp"/>
	<xsl:for-each select="Test">
		_tests["<xsl:value-of select="@Name"/>"] = &amp;<xsl:value-of select="$TestClassName" />::<xsl:value-of select="@Name" />Func;
	</xsl:for-each>
	<xsl:for-each select="Test[@FaultSimulationReady='true']">
		_fsim_tests["<xsl:value-of select="@Name"/>"] = &amp;<xsl:value-of select="$TestClassName" />::<xsl:value-of select="@Name" />Func;
	</xsl:for-each>
	
	//modified	
	struct FSimInfo info;
	info.Point = "kmalloc";
	<xsl:for-each select="FaultSimulation/Simulate">
	<xsl:if test="@point">
	info.Point = "<xsl:value-of select="@point" />";
	</xsl:if>
	<xsl:if test="@count">
	info.Count = <xsl:value-of select="@count" />;
	</xsl:if>
	<xsl:if test="@expression">
	info.Expression = "<xsl:value-of select="@expression" />";
	</xsl:if>
	_fsim_info_vec.push_back(info);
	</xsl:for-each>
	}
	~<xsl:value-of select="$TestClassName" />()
	{
		<xsl:value-of select="CleanUp"/>
	}
	string GetName() { return _name; }
	void ExcludeTest(string test)
	{
		if ( _tests[test] != NULL )
		{
			cerr &lt;&lt; "\n\t Test should be excluded: " &lt;&lt; test &lt;&lt; endl;
			_tests_to_exclude.push_back(test);
		}
		else
			throw Exception("Unknown test " + test);
	}
	void RunTest(string test)
	{
		if ( _tests[test] != NULL )
			_tests_to_run.push_back(test);
		else
			throw Exception("Unknown test " + test);
	}
	
	bool IsTestExcluded(string test)
	{
		return (std::find(_tests_to_exclude.begin(), _tests_to_exclude.end(), test) != _tests_to_exclude.end());
	}
	
	virtual TestResultCollection RunNormalTests()
	{
		TestResultCollection res;
		//for ( unsigned int i  = 0; i &lt; _tests.size(); ++i)
		if ( _tests_to_run.empty() )
		{
			for ( TestMap::iterator it = _tests.begin(); it != _tests.end(); ++it )
			{
				<xsl:value-of select="$ModuleName"/>TestResult * tr = NULL;
				// Check if the test is set to be excluded
				if ( IsTestExcluded(it->first) )
				{
					tr = new <xsl:value-of select="$ModuleName"/>TestResult(new ProcessResult(Skipped, "Test was excluded"), "<xsl:value-of select="$TestSetName" />", it->first);
				}
				else
				{
					ProcessResult * pr = Execute(static_cast&lt;int (Process::*)(vector&lt;string>)>(it->second));
					tr = new <xsl:value-of select="$ModuleName"/>TestResult(pr, "<xsl:value-of select="$TestSetName" />", it->first);
					delete pr;
				}
				res.AddResult( tr );
				// If Fatal error has rised quit!
				if ( tr->GetStatus() == Fatal )
					break;
			}
		}
		else
		{
			for ( unsigned int i = 0; i &lt; _tests_to_run.size(); ++i )
			{				
				ProcessResult * pr = Execute(static_cast&lt;int (Process::*)(vector&lt;string>)>(_tests[_tests_to_run[i]]));
				<xsl:value-of select="$ModuleName"/>TestResult * tr = new <xsl:value-of select="$ModuleName"/>TestResult(pr, "<xsl:value-of select="$TestSetName" />", _tests_to_run[i]);
				delete pr;
				res.AddResult( tr );
			}
		}
		return res;
	}
	virtual TestResultCollection RunFaultyTests()
	{		
		_fsim_enabled = true;
		TestResultCollection res;
		//added
		for ( unsigned int i = 0; i &lt; _fsim_info_vec.size(); ++i )
		{
			_fsim_point = _fsim_info_vec[i].Point;
			_fsim_expression = "0";
				
			 TestMap::iterator it = _fsim_tests.begin();
				ProcessResult * pr = Execute(static_cast&lt;int (Process::*)(vector&lt;string>)>(it->second));
				if ( pr->GetStatus() >= Success &amp;&amp;  pr->GetStatus() &lt;= Fail )
						pr->SetStatus(Success);
					<xsl:value-of select="$ModuleName"/>TestResult * tr = new <xsl:value-of select="$ModuleName"/>TestResult(pr, "<xsl:value-of select="$TestSetName" />", it->first);
					delete pr;
					res.AddResult( tr );
					
					if ( tr->GetStatus() == Signaled )
					{
						DisableFaultSim();
						return res;
					}
			_fsim_info_vec[i].Count = KedrIntegrator::GetTimes(_fsim_info_vec[i].Point);
			cerr &lt;&lt; "<xsl:value-of select="$TestSetName" />: Fault count: " &lt;&lt; _fsim_info_vec[i].Count &lt;&lt; endl;
		}
		
		for ( unsigned int i = 0; i &lt; _fsim_info_vec.size(); ++i )
		{
			cerr &lt;&lt; "<xsl:value-of select="$TestSetName" />: Fault count: " &lt;&lt; _fsim_info_vec[i].Count &lt;&lt; endl;
			_fsim_point = _fsim_info_vec[i].Point;			
			for ( unsigned int j = 1; j &lt; _fsim_info_vec[i].Count+1; ++j ) //modified ( j starts from 1 )
			{
				if ( _fsim_info_vec[i].Expression != "" )
					_fsim_expression = _fsim_info_vec[i].Expression;
				else
				{
					char buf[3];
					sprintf(buf, "%d", j); // modified (i to j) 
					_fsim_expression = "(times%" + static_cast&lt;string>(buf) + " = 0)";
					//_fsim_expression = "times="+static_cast&lt;string>(buf);
				}
				//for ( unsigned int k = 0; k &lt; _fsim_testCount; ++k)
				for ( TestMap::iterator it = _fsim_tests.begin(); it != _fsim_tests.end(); ++it )
				{
					ProcessResult * pr = Execute(static_cast&lt;int (Process::*)(vector&lt;string>)>(it->second));
					if ( pr->GetStatus() >= Success &amp;&amp;  pr->GetStatus() &lt;= Fail )
						pr->SetStatus(Success);
					<xsl:value-of select="$ModuleName"/>TestResult * tr = new <xsl:value-of select="$ModuleName"/>TestResult(pr, "<xsl:value-of select="$TestSetName" />", it->first);
					delete pr;
					res.AddResult( tr );
					// If one of the tests makes the process to get a signal, then the driver probably is not functional any more.
					if ( tr->GetStatus() == Signaled )
					{
						DisableFaultSim();
						return res;
					}
				}
			}
		}
		DisableFaultSim();
		return res;
	}
	<xsl:for-each select="Test">
	int <xsl:value-of select="@Name" />Func(vector&lt;string>)
	{		
		<xsl:value-of select="/TestSet/Header" />
		<xsl:value-of select="Header" />
		cerr &lt;&lt; "Description: " &lt;&lt; "<xsl:value-of select="Description" />" &lt;&lt; endl;
		try
		{
			<xsl:if test="Dir">
			const int DirCount = <xsl:value-of select="Dir/@count"/>;
			string DirPaths[DirCount];
			Directory Dirs[DirCount];
			int DirDs[DirCount];
			<xsl:if test="Dir/File">
			const int DirFileCount = <xsl:value-of select="Dir/File/@count"/>;
			string DirFilePaths[DirFileCount];
			File DirFiles[DirFileCount];
			int DirFDs[DirFileCount];
			</xsl:if>
			for ( int i = 0 ; i &lt; DirCount; ++i )
			{
				char buf[2];
				sprintf(buf, "%d", i);
				DirPaths[i] = DirPrefix + buf;
				DirDs[i] = Dirs[i].Open(DirPaths[i], S_IRWXU);
				if ( DirDs[i] == -1 )
				{
					throw Exception("Directory descriptor is not valid.");
				}
				<xsl:if test="Dir/File">
				for ( int i = 0 ; i &lt; DirFileCount; ++i )
				{
					char buf[2];
					sprintf(buf, "%d", i);
					DirFilePaths[i] = DirPaths[i] + "/" + FilePrefix + buf;
					DirFDs[i] = DirFiles[i].Open(DirFilePaths[i], S_IRWXU, O_CREAT | O_RDWR);
					if ( DirFDs[i] == -1 )
					{
						throw Exception("File descriptor is not valid.");
					}
				}	
				</xsl:if>
			}
			</xsl:if>
			<xsl:if test="File">
			const int FileCount = <xsl:value-of select="File/@count"/>;
			int FileFlags = O_CREAT | O_RDWR;
			int FileMode = S_IRWXU;
			<xsl:if test="File/@flags != ''">
			FileFlags = <xsl:value-of select="File/@flags"/>;
			</xsl:if>
			<xsl:if test="File/@mode != ''">
			FileMode = <xsl:value-of select="File/@mode"/>;
			</xsl:if>
			string FilePaths[FileCount];
			File Files[FileCount];
			int FDs[FileCount];
			for ( int i = 0 ; i &lt; FileCount; ++i )
			{
				char buf[2];
				sprintf(buf, "%d", i);
				FilePaths[i] = FilePrefix + buf;
				FDs[i] = Files[i].Open(FilePaths[i], FileMode, FileFlags);
				if ( FDs[i] == -1 )
				{
					throw Exception("File descriptor is not valid.");
				}
			}	
			</xsl:if>
			
			<xsl:value-of select="Code" />
			
		}
		catch (Exception e)
		{
			Error("Exception was thrown: ", e.GetMessage(), Unresolved);
		}
		
		<xsl:value-of select="Footer" />
		<xsl:value-of select="/TestSet/Footer" />	

	}
	</xsl:for-each>
protected:
	vector&lt;string> _tests_to_run;
	vector&lt;string> _tests_to_exclude;
	string _name;
	bool _fsim_enabled;
	string _fsim_point;
	string _fsim_expression;
	//const unsigned int _testCount;
	//const unsigned int _fsim_testCount;
	TestMap _tests;
	TestMap _fsim_tests;
	int _fault_count;
	//int (<xsl:value-of select="@Name" />Tests::*_tests[<xsl:value-of select="count(Test)"/>])(vector&lt;string>);
	//int (<xsl:value-of select="@Name" />Tests::*_fsim_tests[<xsl:value-of select="count(Test)"/>])(vector&lt;string>);
	vector&lt;FSimInfo> _fsim_info_vec;
	<xsl:value-of select="/TestSet/Internal"/>
	const string DirPrefix;
	const string FilePrefix;
};

<xsl:value-of select="GlobalFooter"/>
	</xsl:template>

</xsl:stylesheet>
