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
#include &lt;Common.hpp>
#include &lt;sys/mount.h>
#include &lt;Test.hpp>
#include &lt;File.hpp>
#include &lt;Directory.hpp>
#include &lt;map>
#include &lt;sys/vfs.h>
#include &lt;XmlSpruceLog.hpp>
#include &lt;vector>
using namespace std;

		<xsl:variable name="ModuleName" select="@Name" />		
class <xsl:value-of select="$ModuleName"/>TestResult : public TestResult 
{
	public:
		
		<xsl:value-of select="$ModuleName"/>TestResult(ProcessResult* pr, string spec, string oper) :	
		TestResult(ProcessResult(*pr), "", spec, oper)
		{						
		}
		virtual string ToXML()
		{			
			stringstream str;
			str &lt;&lt; rand();
			string s = "";
			s += string("&lt;Item Name=\"");
			s += _spec;
			s += string("\" Id=\"");
			s += str.str();
			s += string(" \">\n\t&lt;Operation>");
			s += _operation;
			s += string("&lt;/Operation>\n\t&lt;Status>");
			s += StatusToString();
			s += string("&lt;/Status>\n\t&lt;Output>");
			s += _output;
			s +=  string("&lt;/Output>\n\t&lt;Arguments>&lt;/Arguments>\n\t&lt;/Item>");
			return s;
		}
};


char * DeviceName = NULL;
char * MountPoint = NULL;
char * FileSystem = NULL;
map&lt;string, int> FileSystemTypesMap;



		
		<xsl:for-each select="TestSet">
			<xsl:variable name="TestClassName"><xsl:value-of select="$ModuleName" /><xsl:value-of select="@Name" />Tests</xsl:variable>
			<xsl:variable name="TestSetFile"><xsl:value-of select="$XmlFolder"/>/<xsl:value-of select="@Name"/>.xml</xsl:variable>
			<xsl:variable name="HeaderFile"><xsl:value-of select="$XmlFolder"/>/<xsl:value-of select="@Name"/>.hpp</xsl:variable>
			<exsl:document href="{$HeaderFile}" method="text">
				<xsl:apply-templates select="document($TestSetFile)" >
					<xsl:with-param name="ModuleName" select="$ModuleName"/>
					<xsl:with-param name="TestClassName" select="$TestClassName"/>
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
	
	ModuleLog moduleLog(argv[1]);
	TestResultCollection Results;
	try
	{
			bool SkipTestset;
		<xsl:for-each select="TestSet">
			<xsl:variable name="TestClassName"><xsl:value-of select="$ModuleName" /><xsl:value-of select="@Name" />Tests</xsl:variable>
			<xsl:value-of select="$TestClassName"/><xsl:text> </xsl:text><xsl:value-of select="$ModuleName" /><xsl:value-of select="@Name" />_tests;
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
			}
			if ( !SkipTestset )
			{
			<xsl:choose>
				<xsl:when test="@RunTests">
					Results.Merge(<xsl:value-of select="$ModuleName" /><xsl:value-of select="@Name" />_tests.Run<xsl:value-of select="@RunTests"/>Tests());
				</xsl:when>
				<xsl:otherwise>
					Results.Merge(<xsl:value-of select="$ModuleName" /><xsl:value-of select="@Name" />_tests.RunNormalTests());
				</xsl:otherwise>
			</xsl:choose>
			}
		</xsl:for-each>
			
			//ofstream of(argv[1], ios_base::app);
			//of &lt;&lt; "&lt;Module Name=\"" &lt;&lt; argv[0] &lt;&lt; "\">\n" &lt;&lt; Results.ToXML() &lt;&lt; "&lt;/Module>";
			//of.close();
			
			vector&lt;TestResult*> items = Results.GetResults();
			vector&lt;TestResult*>::iterator i;
			for( i = items.begin(); i != items.end(); i++)
			{
				moduleLog.addItem(Item((*i)->GetSpec(), (*i)->GetStrOperation(), (*i)->StatusToString(), (*i)->GetOutput(), (*i)->GetArguments()));
			}
			return Results.GetStatus();
	}
	catch (Exception ex)
	{
		cerr &lt;&lt; "Exception: " &lt;&lt; ex.GetMessage();
		return EXIT_FAILURE;
	}
}
	</xsl:template>
	
	
	<xsl:template match="/MetaModule">
		<xsl:variable name="RunTests"><xsl:value-of select="@RunTests"/></xsl:variable>
#include &lt;Common.hpp>
#include &lt;sys/mount.h>
#include &lt;Test.hpp>
#include &lt;File.hpp>
#include &lt;Directory.hpp>
#include &lt;map>
#include &lt;sys/vfs.h>
#include &lt;XmlSpruceLog.hpp>

using namespace std;

		<xsl:variable name="MetaModuleName" select="@Name" />


char * DeviceName = NULL;
char * MountPoint = NULL;
char * FileSystem = NULL;
map&lt;string, int> FileSystemTypesMap;



		<xsl:for-each select="Module">
			<xsl:variable name="ModuleName"><xsl:value-of select="@Name"/></xsl:variable>
			<xsl:variable name="ModulePath"><xsl:value-of select="@Path"/></xsl:variable>
			
class <xsl:value-of select="$ModuleName"/>TestResult : public TestResult 
{
	public:
		
		<xsl:value-of select="$ModuleName"/>TestResult(ProcessResult* pr, string spec, string oper) :	
		TestResult(ProcessResult(*pr), "", ""), _spec(spec), _operation(oper)
		{						
		}
		virtual string ToXML()
		{
			
			stringstream str;
			str &lt;&lt; rand();
			
			return "&lt;Item Name=\"" + _spec + "\" Id=\"" + str.str() + " \">" + "\n\t&lt;Operation>" + _operation + "&lt;/Operation>\n\t&lt;Status>" + StatusToString() + "&lt;/Status>\n\t&lt;Output>" +	_output +  "&lt;/Output>\n\t&lt;Arguments>" + "" + "&lt;/Arguments>" + "\n\t" +  "&lt;/Item>";
		}
	protected:
		string _spec;
		string _operation;
};

			<xsl:variable name="ModuleFile">../src/<xsl:value-of select="@Path"/>/module.xml</xsl:variable>
			<xsl:variable name="Path">../<xsl:value-of select="@Path"/></xsl:variable>
			// Processing Module <xsl:value-of select="@Name"/> from <xsl:value-of select="@Path"/>
			<xsl:for-each select="document($ModuleFile)/Module/TestSet">
				<xsl:variable name="HeaderFile"><xsl:value-of select="$Path"/>/<xsl:value-of select="@Name"/>.hpp</xsl:variable>				
#include &lt;<xsl:value-of select="$HeaderFile"/>>
			</xsl:for-each>
		</xsl:for-each>
		
		
		<!--xsl:for-each select="TestSet">
				using <xsl:value-of select="@Name"/>
		</xsl:for-each-->
		
int main(int argc, char ** argv)
{
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
		<xsl:for-each select="Module">
			<xsl:variable name="ModuleName" select="@Name" />
			<xsl:variable name="ModuleFile">../src/<xsl:value-of select="@Path"/>/module.xml</xsl:variable>			
			<xsl:for-each select="document($ModuleFile)/Module/TestSet">
				<xsl:variable name="TestClassName"><xsl:value-of select="$ModuleName" /><xsl:value-of select="@Name" />Tests</xsl:variable>
				<xsl:value-of select="$TestClassName" /><xsl:text> </xsl:text><xsl:value-of select="$ModuleName" /><xsl:value-of select="@Name" />_tests;
				<xsl:choose>
					<xsl:when test="$RunTests!=''">
						Results.Merge(<xsl:value-of select="$ModuleName" /><xsl:value-of select="@Name" />_tests.Run<xsl:value-of select="$RunTests"/>Tests());
					</xsl:when>
					<xsl:otherwise>
						Results.Merge(<xsl:value-of select="$ModuleName" /><xsl:value-of select="@Name" />_tests.RunNormalTests());
					</xsl:otherwise>
				</xsl:choose>
			</xsl:for-each>
		</xsl:for-each>
		
		if ( argc == 2 )
		{
			//ofstream of(argv[1], ios_base::app);
			//of &lt;&lt; "&lt;Module Name=\"<xsl:value-of select="@Name"/>\">\n" &lt;&lt; Results.ToXML() &lt;&lt; "&lt;/Module>";
			//of.close();
			ModuleLog moduleLog(argv[1]);
			vector&lt;TestResult*> items = Results.GetResults();
			vector&lt;TestResult*>::iterator i;
			for( i = items.begin(); i != items.end(); i++)
			{
				moduleLog.addItem(Item((*i)->GetSpec(), (*i)->GetStrOperation(), (*i)->StatusToString(), (*i)->GetOutput(), (*i)->GetArguments()));
			}
			return Results.GetStatus();
		}
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
