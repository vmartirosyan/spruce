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
	<xsl:value-of select="GlobalHeader"/>
	<xsl:for-each select="Requires">
#include &lt;<xsl:value-of select="." />>
	</xsl:for-each> 
	
#include &lt;process.hpp>
	<xsl:variable name="TestSetName" select="@Name" />
	

class <xsl:value-of select="@Name" />Tests : public Process
{
public:
	<xsl:value-of select="@Name" />Tests():
		_testCount(<xsl:value-of select="count(Test)"/>)
	{
		Process::EnableAlarm = true;
		<xsl:value-of select="StartUp"/>
		<xsl:for-each select="Test">
		_tests[<xsl:value-of select="position()-1"/>] = &amp;<xsl:value-of select="$TestSetName" />Tests::<xsl:value-of select="@Name" />Func;
		</xsl:for-each>
	}
	~<xsl:value-of select="@Name" />Tests()
	{
		<xsl:value-of select="CleanUp"/>
	}
	
	virtual TestResultCollection RunTests()
	{
		TestResultCollection res;
		for ( unsigned int i  = 0; i &lt; _testCount; ++i)
		{
			ProcessResult * pr = Execute((int (Process::*)(vector&lt;string>))_tests[i]);
			<xsl:value-of select="$ModuleName"/>TestResult * tr = new <xsl:value-of select="$ModuleName"/>TestResult(pr, "<xsl:value-of select="$TestSetName" />");
			delete pr;
			res.AddResult( tr );
		}
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
				DirPaths[i] = "<xsl:value-of select="@Name" />_dir_" + (string)buf;
				DirDs[i] = Dirs[i].Open(DirPaths[i], S_IRWXU);
				<xsl:if test="Dir/File">
				for ( int i = 0 ; i &lt; DirFileCount; ++i )
				{
					char buf[2];
					sprintf(buf, "%d", i);
					DirFilePaths[i] = DirPaths[i] + "/file_" + (string)buf;
					DirFDs[i] = DirFiles[i].Open(DirFilePaths[i], S_IRWXU, O_CREAT | O_RDWR);
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
				FilePaths[i] = "<xsl:value-of select="@Name" />_file_" + (string)buf;
				FDs[i] = Files[i].Open(FilePaths[i], FileMode, FileFlags);
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
	const unsigned int _testCount;
	int (<xsl:value-of select="@Name" />Tests::*_tests[<xsl:value-of select="count(Test)"/>])(vector&lt;string>);
	<xsl:value-of select="/TestSet/Internal"/>
};

<xsl:value-of select="GlobalFooter"/>
	</xsl:template>

</xsl:stylesheet>