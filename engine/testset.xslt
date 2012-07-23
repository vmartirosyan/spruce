<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" encoding="utf-8" />
	

	<xsl:template match="/TestSet">
		<xsl:param name="ModuleName" />
	<xsl:for-each select="Requires">
#include &lt;<xsl:value-of select="." />>
	</xsl:for-each> 
	
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
			string dir_path = "<xsl:value-of select="@Name" />_dir";
			string file_path = dir_path + "/_file";
			Directory dir(dir_path.c_str(), S_IRWXU);
			File file(file_path.c_str());
			
			int fd = file.GetFileDescriptor();
			if (fd == -1)
			{
				cerr &lt;&lt; "Cannot obtain file descriptor";
				return Unres;
			}
			
			<xsl:value-of select="Code" />
			
		}
		catch (Exception e)
		{
			cerr &lt;&lt; "Exception was thrown: " &lt;&lt; e.GetMessage();
			return Unres;
		}
		
		<xsl:value-of select="Footer" />
			

	}
	</xsl:for-each>
protected:
	const unsigned int _testCount;
	int (<xsl:value-of select="@Name" />Tests::*_tests[<xsl:value-of select="count(Test)"/>])(vector&lt;string>);
};
	</xsl:template>

</xsl:stylesheet>
