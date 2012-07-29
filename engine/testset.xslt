<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" encoding="utf-8" />
	

<xsl:template match="/TestSet">
		<xsl:param name="ModuleName" />
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
		
		<xsl:for-each select="Test">
		_tests[<xsl:value-of select="position()-1"/>] = &amp;<xsl:value-of select="$TestSetName" />Tests::<xsl:value-of select="@Name" />Func;
		</xsl:for-each>
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
		<xsl:value-of select="Header" />
		cerr &lt;&lt; "Description: " &lt;&lt; "<xsl:value-of select="Description" />" &lt;&lt; endl;
		try
		{
			<xsl:if test="Dir">
			const int DirCount = <xsl:value-of select="Dir/@count"/>;
			string DirPaths[DirCount];
			Directory Dirs[DirCount];
			for ( int i = 0 ; i &lt; DirCount; ++i )
			{
				char buf[2];
				sprintf(buf, "%d", i);
				DirPaths[i] = "<xsl:value-of select="@Name" />_dir_" + (string)buf;
				Dirs[i].Open(DirPaths[i], S_IRWXU);
			}
			</xsl:if>
			<xsl:if test="File">
			const int FileCount = <xsl:value-of select="File/@count"/>;
			string FilePaths[FileCount];
			File Files[FileCount];
			int FDs[FileCount];
			for ( int i = 0 ; i &lt; FileCount; ++i )
			{
				char buf[2];
				sprintf(buf, "%d", i);
				FilePaths[i] = "<xsl:value-of select="@Name" />_file_" + (string)buf;
				FDs[i] = Files[i].Open(FilePaths[i], S_IRWXU, O_CREAT | O_RDWR);
			}	
			</xsl:if>
			
			<xsl:value-of select="Code" />
			
		}
		catch (Exception e)
		{
			Error("Exception was thrown: ", e.GetMessage(), Unresolved);
		}
		
		<xsl:value-of select="Footer" />
			

	}
	</xsl:for-each>
protected:
	const unsigned int _testCount;
	int (<xsl:value-of select="@Name" />Tests::*_tests[<xsl:value-of select="count(Test)"/>])(vector&lt;string>);
	<xsl:value-of select="/TestSet/Internal"/>
};
	</xsl:template>

</xsl:stylesheet>
