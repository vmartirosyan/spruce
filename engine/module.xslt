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
#include &lt;common.hpp>
#include &lt;sys/mount.h>
#include &lt;test.hpp>
#include &lt;File.hpp>
#include &lt;Directory.hpp>
#include &lt;map>
#include &lt;sys/vfs.h>

using namespace std;

		<xsl:variable name="ModuleName" select="@Name" />
class <xsl:value-of select="$ModuleName"/>TestResult : public TestResult 
{
	public:
		
		<xsl:value-of select="$ModuleName"/>TestResult(ProcessResult* pr, string spec) :	
		TestResult(ProcessResult(*pr), "", ""), _spec(spec)
		{						
		}
		virtual string ToXML()
		{
			
			stringstream str;
			str &lt;&lt; rand();
			
			return "&lt;Item Name=\"" + _spec + "\" Id=\"" + str.str() + " \">" + "\n\t&lt;Operation>" + "Unknown" + "&lt;/Operation>\n\t&lt;Status>" + StatusToString() + "&lt;/Status>\n\t&lt;Output>" +	_output +  "&lt;/Output>\n\t&lt;Arguments>" + "" + "&lt;/Arguments>" + "\n\t" +  "&lt;/Item>";
		}
	protected:
		string _spec;
};

class <xsl:value-of select="$ModuleName"/>Test : public Test
{
	public:		
		<xsl:value-of select="$ModuleName"/>Test(Mode mode, int operation, string arguments, string spec) : 
		Test(mode, operation, arguments), _spec(spec) 
		{
		}
		
		<xsl:value-of select="$ModuleName"/>Test(Mode mode, string operation, string arguments, string spec) : 
		Test(mode, operation, arguments), _spec(spec) 
		{
		}														
		
		string GetSpec() const 
		{
			return _spec;
		}				
		
		virtual <xsl:value-of select="$ModuleName"/>TestResult* Execute(vector&lt;string> args)
		{
			TestResult* tr = (TestResult*)Test::Execute(args);						
			<xsl:value-of select="$ModuleName"/>TestResult* ModuleTestResult = new <xsl:value-of select="$ModuleName"/>TestResult(tr, _spec);			
			delete tr;
			return ModuleTestResult;			
		}
		
	protected:
		string _spec;
};

char * DeviceName = (char*)"";
char * MountPoint = (char*)"";
char * FileSystem = (char*)"";
map&lt;string, int> FileSystemTypesMap;



		
		<xsl:for-each select="TestSet">
			<xsl:variable name="TestSetFile"><xsl:value-of select="$XmlFolder"/>/<xsl:value-of select="@Name"/>.xml</xsl:variable>
			<xsl:variable name="HeaderFile"><xsl:value-of select="$XmlFolder"/>/<xsl:value-of select="@Name"/>.hpp</xsl:variable>
			<exsl:document href="{$HeaderFile}" method="text">
				<xsl:apply-templates select="document($TestSetFile)" >
					<xsl:with-param name="ModuleName" select="$ModuleName"/>
				</xsl:apply-templates>
			</exsl:document>
#include "<xsl:value-of select="$HeaderFile"/>"
		</xsl:for-each>
		
		<!--xsl:for-each select="TestSet">
				using <xsl:value-of select="@Name"/>
		</xsl:for-each-->
		
int main(int argc, char ** argv)
{
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
		
	TestResultCollection Results;
	try
	{
		<xsl:for-each select="TestSet">
			<xsl:value-of select="@Name" />Tests <xsl:value-of select="@Name" />_tests;
			<xsl:choose>
				<xsl:when test="@RunTests">
					Results.Merge(<xsl:value-of select="@Name" />_tests.Run<xsl:value-of select="@RunTests"/>Tests());
				</xsl:when>
				<xsl:otherwise>
					Results.Merge(<xsl:value-of select="@Name" />_tests.RunNormalTests());
				</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>
		
		ofstream of(argv[1], ios_base::app);
		
		of &lt;&lt; "&lt;Module Name=\"<xsl:value-of select="@Name"/>\">\n" &lt;&lt; Results.ToXML() &lt;&lt; "&lt;/Module>";
		
		of.close();
		
		return Results.GetStatus();
	}
	catch (Exception ex)
	{
		cerr &lt;&lt; "Exception: " &lt;&lt; ex.GetMessage();
		return EXIT_FAILURE;
	}
}
	</xsl:template>

</xsl:stylesheet>
