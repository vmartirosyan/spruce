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
#include &lt;memory>
#include &lt;UnixCommand.hpp>
#include &lt;kedr_integrator.hpp>
#include &lt;PartitionManager.hpp>
using std::map;
using std::string;
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
	
	struct FSimInfo info;
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
	if( _fsim_info_vec.empty() &amp;&amp;  !_fsim_tests.empty() )
	{
		info.Point = "kmalloc";
		_fsim_info_vec.push_back(info);
	}
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
				std::auto_ptr&lt;ProcessResult> pr;
				if ( IsTestExcluded(it->first) )
				{
					tr = new <xsl:value-of select="$ModuleName"/>TestResult(new ProcessResult(Skipped, "Test was excluded"), "<xsl:value-of select="$TestSetName" />", it->first);
				}
				else
				{
					pr = std::auto_ptr&lt;ProcessResult>( Execute(static_cast&lt;int (Process::*)(vector&lt;string>)>(it->second)) );
					tr = new <xsl:value-of select="$ModuleName"/>TestResult(pr.get(), "<xsl:value-of select="$TestSetName" />", it->first);
				
				}

				string log;
				Status oopsStatus = OopsChecker(log); // log is an output parameter
				if(oopsStatus != Success) 
				{	
					//so we have an emergency situation...
					cerr&lt;&lt;"Oops Checker is activated: \n";
					log = "Status: " + StatusMessages[oopsStatus] + "\nTest output: \n" + pr->GetOutput() + "\nSystem log message: \n" + log;
					tr = new <xsl:value-of select="$ModuleName"/>TestResult(new ProcessResult(Fatal, log),"<xsl:value-of select="$TestSetName" />", it->first);
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
		cerr &lt;&lt; "Running faulty tests:  <xsl:value-of select="$TestSetName" />: " &lt;&lt; _fsim_info_vec.size() &lt;&lt; endl;
		_fsim_enabled = true;
		TestResultCollection res;
	
		for ( unsigned int i = 0; i &lt; _fsim_info_vec.size(); ++i )
		{
			_fsim_point = _fsim_info_vec[i].Point;
			_fsim_expression = "0";
			//KedrIntegrator::SetIndicator(_fsim_point, "common", _fsim_expression);	
			
			 TestMap::iterator it = _fsim_tests.begin();
			 cerr &lt;&lt; "\033[1;31mCalling: Test name: " &lt;&lt; it->first &lt;&lt; "\t. Parent pid: " &lt;&lt; getpid() &lt;&lt; ".\033[0m" &lt;&lt; endl;
				ProcessResult * pr = Execute((int (Process::*)(vector&lt;string>))it->second);
				//if ( pr->GetStatus() >= Success &amp;&amp;  pr->GetStatus() &lt;= Fail )
						//pr->SetStatus(Success);
					cerr &lt;&lt; "\033[1;31mCalled: Test name: " &lt;&lt; it->first &lt;&lt; "\t. Parent pid: " &lt;&lt; getpid() &lt;&lt; ".\033[0m" &lt;&lt; endl;
					<xsl:value-of select="$ModuleName"/>TestResult * tr = new <xsl:value-of select="$ModuleName"/>TestResult(pr, "<xsl:value-of select="$TestSetName" />", it->first);
					delete pr;
					res.AddResult( tr );
					
					if ( tr->GetStatus() == Signaled )
					{
						DisableFaultSim();
						return res;
					}
			
			_fsim_info_vec[i].Count = KedrIntegrator::GetTimes(_fsim_point);
			KedrIntegrator:: ResetTimes(_fsim_point);
			//DisableFaultSim();
			cerr &lt;&lt; "<xsl:value-of select="$TestSetName" />: Fault count: " &lt;&lt; _fsim_info_vec[i].Count &lt;&lt; endl;
		}
		
		
		for ( TestMap::iterator it = _fsim_tests.begin(); it != _fsim_tests.end(); ++it )
		for ( unsigned int i = 0; i &lt; _fsim_info_vec.size(); ++i )
		{
			cerr &lt;&lt; "<xsl:value-of select="$TestSetName" />: Fault count: " &lt;&lt; _fsim_info_vec[i].Count &lt;&lt; endl;
			_fsim_point = _fsim_info_vec[i].Point;			
			for ( unsigned int j = 1; j &lt; _fsim_info_vec[i].Count+1; ++j ) 
			{
				if ( _fsim_info_vec[i].Expression != "" )
					_fsim_expression = _fsim_info_vec[i].Expression;
				else
				{
					char buf[10];
					sprintf(buf, "%d", j);  
					//_fsim_expression = "(times%" + (string)buf + " = 0)";
					_fsim_expression = "times="+(string)buf;
					
				}
				//for ( unsigned int k = 0; k &lt; _fsim_testCount; ++k)
				
					ProcessResult * pr = Execute((int (Process::*)(vector&lt;string>))it->second);
					//if ( pr->GetStatus() >= Success &amp;&amp;  pr->GetStatus() &lt;= Fail )
						//pr->SetStatus(Success);
					if( _fsim_info_vec[i].Count > 0 &amp;&amp;  pr->GetStatus() == Success )
					{
						pr->SetStatus(FSimSuccess);
						pr->ModOutput("Test returned Success instead of Fail. Fault Simulation failed.");	
					}
					if( _fsim_info_vec[i].Count > 0 &amp;&amp; pr->GetStatus() == Fail )
						pr->SetStatus(FSimFail);
					<xsl:value-of select="$ModuleName"/>TestResult * tr = new <xsl:value-of select="$ModuleName"/>TestResult(pr, "<xsl:value-of select="$TestSetName" />", it->first);
					delete pr;
					res.AddResult( tr );
					// If one of the tests makes the process to get a signal, then the driver probably is not functional any more.
					if ( tr->GetStatus() == Signaled )
					{
						DisableFaultSim();
						return res;
					}
				
				//DisableFaultSim();
			}
			//DisableFaultSim();	
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
	Status OopsChecker(string&amp; OutputLog)	
	{
		string mainMessage;
		vector&lt;string> args;
		args.push_back("-c");
		args.push_back("dmesg");
		UnixCommand* command = new UnixCommand("dmesg");
		std::auto_ptr&lt;ProcessResult> result(command->Execute());
		if(result.get() == NULL || result->GetStatus() != Success)
		{
			OutputLog = "Unable to read system log";
			if ( result.get() != NULL )
				OutputLog += result->GetOutput();
			return Unresolved;
		}
		mainMessage = result->GetOutput();
		delete command;
		
		//searching points
		const string bug = "BUG";
		const string oops = "Oops";
		const string panic = "ernel panic";  //interested in both "Kernel panic" and "kernel panic"
		const char less = 60;  // 60 is the ASCII code of the operation 'less than'
		const char amp = 38;   // 38 is the ASCII code of the operation 'ampersand'
		size_t foundPos;
		
		foundPos = mainMessage.find(bug); // searching "bug" in the kernel output
		if( foundPos != string::npos )
		{
			OutputLog.assign(mainMessage.begin() + foundPos, mainMessage.end());
			size_t pos = 0;
			while(true)
			{
				pos = (OutputLog.find(amp, pos));
				if(pos == std::string::npos)
					break;
				OutputLog.replace(pos, 1, "&amp;amp;"); 
				++pos;
			}
			while(true)
			{
				pos = (OutputLog.find(less, pos));
				if(pos == std::string::npos)
					break;
				OutputLog.replace(pos, 1, "&amp;lt;");   
				++pos;
			}
			cerr &lt;&lt; "Oops checker: bug found" &lt;&lt; endl;
			return Bug;
		}
		foundPos = mainMessage.find(oops);			
		if( foundPos != string::npos )
		{
			OutputLog.assign(mainMessage.begin() + foundPos, mainMessage.end());
			size_t pos = 0;
			while(true)
			{
				pos = (OutputLog.find(amp, pos));
				if(pos == std::string::npos)
					break;
				OutputLog.replace(pos, 1, "&amp;amp;");  
				++pos;
			}
			while(true)
			{
				pos = (OutputLog.find(less, pos));
				if(pos == std::string::npos)
					break;
				OutputLog.replace(pos, 1, "&amp;lt;");  
				++pos;
			}
			cerr &lt;&lt; "Oops checker: oops found" &lt;&lt; endl;
			return Oops;
		}
		foundPos = mainMessage.find(panic);			
		if( foundPos != string::npos )
		{
			OutputLog.assign(mainMessage.begin() + foundPos, mainMessage.end());
			size_t pos = 0;
			while(true)
			{
				pos = (OutputLog.find(amp, pos));
				if(pos == std::string::npos)
					break;
				OutputLog.replace(pos, 1, "&amp;amp;");   // ???
				++pos;
			}
			while(true)
			{
				pos = (OutputLog.find(less, pos));
				if(pos == std::string::npos)
					break;
				OutputLog.replace(pos, 1, "&amp;lt;");  
				++pos;
			}
			cerr &lt;&lt; "Oops checker: Panic found" &lt;&lt; endl;
			return Panic;
		}
		return Success;
	}
};

<xsl:value-of select="GlobalFooter"/>
	</xsl:template>

</xsl:stylesheet>
