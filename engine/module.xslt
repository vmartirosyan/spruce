<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" encoding="utf-8" indent="yes"/>
<xsl:include href="testset.xslt" />
	

	<xsl:template match="/Module">
		#include &lt;common.hpp>
		#include &lt;config.hpp>
		#include &lt;time.h>
		#include &lt;test.hpp>
		#include &lt;File.hpp>
		#include &lt;Directory.hpp>
		
		<xsl:variable name="ModuleName" select="@Name" />
		
		class <xsl:value-of select="$ModuleName"/>TestResult : public TestResult 
		{
			public:
				<xsl:value-of select="$ModuleName"/>TestResult(TestResult* tr, string spec) :	
				TestResult(*tr), _spec(spec)
				{						
				}
				virtual string ToXML()
				{
					
					stringstream str;
					str &lt;&lt; rand();
					
					return "&lt;Item Name=\"" + _spec + "\" Id=\"" + str.str() + " \">" + "\n\t&lt;Operation>" + _stroperation + "&lt;/Operation>\n\t&lt;Status>" + StatusToString() + "&lt;/Status>\n\t&lt;Output>" +	_output +  "&lt;/Output>\n\t&lt;Arguments>" + _arguments + "&lt;/Arguments>" + "\n\t" +  "&lt;/Item>";
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
		
		string DeviceName = "";
		string MountPoint = "";
		
		<xsl:for-each select="TestSet">
			<xsl:variable name="TestSetFile"><xsl:value-of select="$XmlFolder"/>/<xsl:value-of select="@Name"/>.xml</xsl:variable>
			//proxcessing file <xsl:value-of select="$TestSetFile" />
			<xsl:apply-templates select="document($TestSetFile)" >
				<xsl:with-param name="ModuleName" select="$ModuleName"/>
			</xsl:apply-templates>
		</xsl:for-each>
		
		<!--xsl:for-each select="TestSet">
				using <xsl:value-of select="@Name"/>
		</xsl:for-each-->
		
		int main()
		{
			if ( getenv("Partition") )	
				DeviceName = getenv("Partition");
	
			if ( getenv("MountAt") )				
				MountPoint = getenv("MountAt");	
			
			TestCollection tests;
			try
			{
				<xsl:for-each select="TestSet">
					<xsl:value-of select="@Name" />::<xsl:value-of select="@Name" />Collection <xsl:value-of select="@Name" />_tests;
					tests.Merge(<xsl:value-of select="@Name" />_tests);
				</xsl:for-each>
			
				TestResultCollection res = tests.Run();
	
				cerr &lt;&lt; "&lt;Module Name=\"<xsl:value-of select="@Name"/>\">\n" &lt;&lt; res.ToXML() &lt;&lt; "&lt;/Module>" &lt;&lt; endl;
			}
			catch (Exception ex)
			{
				cerr &lt;&lt; "Exception: " &lt;&lt; ex.GetMessage();
				return EXIT_FAILURE;
			}
		}
	</xsl:template>

</xsl:stylesheet>
