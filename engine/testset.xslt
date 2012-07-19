<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" encoding="utf-8" />
	

	<xsl:template match="/TestSet">
		<xsl:param name="ModuleName" />
	<xsl:for-each select="Test/Requires/Header">
	#include &lt;<xsl:value-of select="." />>
	</xsl:for-each> 
	
	<xsl:variable name="TestSetName" select="@Name" />
	
	namespace <xsl:value-of select="@Name" />
	{
		<xsl:for-each select="Test">
		class <xsl:value-of select="@Name" />Test : public <xsl:value-of select="$ModuleName" />Test
		{
		public:
			<xsl:value-of select="@Name" />Test(Mode m, int op, string a): <xsl:value-of select="$ModuleName" />Test(m,op,a, "<xsl:value-of select="$TestSetName" />")
			{}			
			
			<xsl:value-of select="@Name" />Test(Mode m, string op, string a): <xsl:value-of select="$ModuleName" />Test(m,op,a, "<xsl:value-of select="$TestSetName" />")
			{}			
			
		protected:
			virtual int Main(vector&lt;string> args)
			{
				cerr &lt;&lt; "Description: " &lt;&lt; "<xsl:value-of select="Description" />";
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
					
					<xsl:value-of select="Code/Main" />
					
				}
				catch (Exception e)
				{
					cerr &lt;&lt; "Exception was thrown: " &lt;&lt; e.GetMessage();
					return Unres;
				}
				
				<xsl:value-of select="Code/Finally" />
				
			}
		};
		</xsl:for-each>
		
		class <xsl:value-of select="$TestSetName" />Collection : public TestCollection
		{
			public:
				<xsl:value-of select="$TestSetName" />Collection()
				{
				<xsl:for-each select="Test">
					AddTest(new <xsl:value-of select="@Name" />Test(Normal, "<xsl:value-of select="@Name" />", ""));
				</xsl:for-each>		
				}
		};
		
	}	
	</xsl:template>

</xsl:stylesheet>
