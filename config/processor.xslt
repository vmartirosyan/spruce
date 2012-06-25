<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output
    method="html"    
    doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN"
    indent="yes"
    encoding="utf-8" />
    
<xsl:key name="Syscalls" match="//SysCall" use="@Name" />
<xsl:key name="Ext4FSs" match="//Ext4FS" use="@Name" />
<xsl:key name="JFSs" match="//JFS" use="@Name" />

<xsl:template match="/SpruceLog">
<html>
	<head>
		<style>
			.Success
			{
				color:Green;
			}
			.Unresolved
			{
				color:Orange;
			}
			.Failed
			{
				color:Red;
			}
			.Unsupported
			{
				color:Gray;
			}
		</style>
		<title>Spruce log for filesystem <xsl:value-of select="//FS/@Name" /></title>
	</head>
	<body>
		<script language="javascript">
			function ShowHideTest(object, ID)
			{					
				document.getElementById(ID).style.display = 
					(document.getElementById(ID).style.display == "none" ? "" : "none");
				object.innerText = (object.innerText == ' -' ? ' +' : ' -');
			}
		</script>
		<!--xsl:for-each select="//FS"-->
			
			<xsl:variable name="fs" select="//FS/@Name" />
			<h1>Filesystem: <xsl:value-of select="$fs" /></h1>
			<H2>Module: Syscalls</H2>
			
			<xsl:for-each select="//SysCall[count(. | key('Syscalls', @Name)[1]) = 1]">
			
				
				<xsl:sort select="@Name" />
				
				<xsl:variable name="SysCall" select="@Name" />
				<xsl:variable name="TestsTotal" select="count(//SysCall[@Name=$SysCall])" />
				<xsl:variable name="TestsPassed" select="count(//SysCall[@Name=$SysCall and Status='Success'])" />
				<span class="Failed">
					<xsl:if test="$TestsPassed=$TestsTotal">
						<xsl:attribute name="class">					
							Success
						</xsl:attribute>
					</xsl:if>
				<xsl:value-of select="$SysCall" />			
				(Passed : <xsl:value-of select="$TestsPassed" />/<xsl:value-of select="$TestsTotal" />)
				</span>
				<a href="#" style="text-decoration:none">
				<xsl:attribute name="onclick">ShowHideTest(this, 'SysCall_<xsl:value-of select="$SysCall" />')</xsl:attribute> +</a>
				<br />
				<div style="border: solid 1px black; padding: 5px; display:none;">	
					<xsl:attribute name="id">SysCall_<xsl:value-of select="$SysCall" /></xsl:attribute>
					<xsl:for-each select="//SysCall[@Name=$SysCall]">
						<div>
							<xsl:attribute name="class"><xsl:value-of select="Status" /></xsl:attribute>
							<xsl:value-of select="Operation" />: <xsl:value-of select="Status" /><br />
							<xsl:if test="Status!='Success'">----> Output: <xsl:value-of select="Output" /></xsl:if>
						</div>
					</xsl:for-each>
					<hr />
					Total Tests: <xsl:value-of select="count(//SysCall[@Name=$SysCall])" /><br />
					<span class="Success">Tests passed: <xsl:value-of select="count(//SysCall[@Name=$SysCall and Status='Success'])" /></span> <br/>
					<span class="Failed">Tests failed: <xsl:value-of select="count(//SysCall[@Name=$SysCall and Status='Failed'])" /></span> <br/>
					<span class="Unresolved">Tests unresolved: <xsl:value-of select="count(//SysCall[@Name=$SysCall and Status='Unresolved'])" /></span> <br/>
					<span class="Unsupported">Tests unsupported: <xsl:value-of select="count(//SysCall[@Name=$SysCall and Status='Unsupported'])" /></span> <br/>
				</div>
			</xsl:for-each>	
			
			<hr />
			
			<span class="Success">Total tests passed: <xsl:value-of select="count(//SysCall[Status='Success'])" /></span> <br/>
			<span class="Failed">Total tests failed: <xsl:value-of select="count(//SysCall[Status='Failed'])" /></span>
			<a href="#" style="text-decoration:none">
				<xsl:attribute name="onclick">ShowHideTest(this, 'SyscallTestsFailed')</xsl:attribute> +</a>
			<div id="SyscallTestsFailed"  style="border: solid 1px black; padding: 5px; display:none;" class="Failed">
				<xsl:for-each select="//SysCall[Status='Failed']" >
					<xsl:value-of select="@Name" /> : <xsl:value-of select="Operation" /> : <xsl:value-of select="Output" /><br />
				</xsl:for-each>
			</div>
			<br />
			<span class="Unresolved">Total tests unresolved: <xsl:value-of select="count(//SysCall[Status='Unresolved'])" /></span>
			<a href="#" style="text-decoration:none">
				<xsl:attribute name="onclick">ShowHideTest(this, 'SyscallTestsUnresolved')</xsl:attribute> +</a>
			<div id="SyscallTestsUnresolved"  style="border: solid 1px black; padding: 5px; display:none;" class="Unresolved">
				<xsl:for-each select="//SysCall[Status='Unresolved']" >
					<xsl:value-of select="@Name" /> : <xsl:value-of select="Operation" /> : <xsl:value-of select="Output" /><br />
				</xsl:for-each>
			</div>
			<br />
			<span class="Unsupported">Total tests unsupported: <xsl:value-of select="count(//SysCall[Status='Unsupported'])" /></span>
			<a href="#" style="text-decoration:none">
				<xsl:attribute name="onclick">ShowHideTest(this, 'SyscallTestsUnsupported')</xsl:attribute> +</a>
			<div id="SyscallTestsUnsupported"  style="border: solid 1px black; padding: 5px; display:none;" class="Unsupported">
				<xsl:for-each select="//SysCall[Status='Unsupported']" >
					<xsl:value-of select="@Name" /> : <xsl:value-of select="Operation" /> : <xsl:value-of select="Output" /><br />
				</xsl:for-each>
			</div>
			
			<H2>Module: Ext4FS</H2>
			<xsl:for-each select="//Ext4FS[count(. | key('Ext4FSs', @Name)[1]) = 1]">
							
				<xsl:sort select="@Name" />
				
				<xsl:variable name="Ext4FS" select="@Name" />
				<xsl:variable name="TestsTotal" select="count(//Ext4FS[@Name=$Ext4FS])" />
				<xsl:variable name="TestsPassed" select="count(//Ext4FS[@Name=$Ext4FS and Status='Success'])" />
				<span class="Failed">
					<xsl:if test="$TestsPassed=$TestsTotal">
						<xsl:attribute name="class">					
							Success
						</xsl:attribute>
					</xsl:if>
				<xsl:value-of select="$Ext4FS" />			
				(Passed : <xsl:value-of select="$TestsPassed" />/<xsl:value-of select="$TestsTotal" />)
				</span>
				<a href="#" style="text-decoration:none">
				<xsl:attribute name="onclick">ShowHideTest(this, 'Ext4FS_<xsl:value-of select="$Ext4FS" />')</xsl:attribute> +</a>
				<br />
				<div style="border: solid 1px black; padding: 5px; display:none;">	
					<xsl:attribute name="id">Ext4FS_<xsl:value-of select="$Ext4FS" /></xsl:attribute>
					<xsl:for-each select="//Ext4FS[@Name=$Ext4FS]">
						<div>
							<xsl:attribute name="class"><xsl:value-of select="Status" /></xsl:attribute>
							<xsl:value-of select="Operation" />: <xsl:value-of select="Status" /><br />
							<xsl:if test="Status!='Success'">----> Output: <xsl:value-of select="Output" /></xsl:if>
						</div>
					</xsl:for-each>
					<hr />
					Total Tests: <xsl:value-of select="count(//Ext4FS[@Name=$Ext4FS])" /><br />
					<span class="Success">Tests passed: <xsl:value-of select="count(//Ext4FS[@Name=$Ext4FS and Status='Success'])" /></span> <br/>
					<span class="Failed">Tests failed: <xsl:value-of select="count(//Ext4FS[@Name=$Ext4FS and Status='Failed'])" /></span> <br/>
					<span class="Unresolved">Tests unresolved: <xsl:value-of select="count(//Ext4FS[@Name=$Ext4FS and Status='Unresolved'])" /></span> <br/>
					<span class="Unsupported">Tests unsupported: <xsl:value-of select="count(//Ext4FS[@Name=$Ext4FS and Status='Unsupported'])" /></span> <br/>
				</div>
			</xsl:for-each>	
			
			<hr />
			
			<span class="Success">Total tests passed: <xsl:value-of select="count(//Ext4FS[Status='Success'])" /></span> <br/>
			<span class="Failed">Total tests failed: <xsl:value-of select="count(//Ext4FS[Status='Failed'])" /></span>
			<a href="#" style="text-decoration:none">
				<xsl:attribute name="onclick">ShowHideTest(this, 'Ext4FSTestsFailed')</xsl:attribute> +</a>
			<div id="Ext4FSTestsFailed"  style="border: solid 1px black; padding: 5px; display:none;" class="Failed">
				<xsl:for-each select="//Ext4FS[Status='Failed']" >
					<xsl:value-of select="@Name" /> : <xsl:value-of select="Operation" /> : <xsl:value-of select="Output" /><br />
				</xsl:for-each>
			</div>
			<br />
			<span class="Unresolved">Total tests unresolved: <xsl:value-of select="count(//Ext4FS[Status='Unresolved'])" /></span>
			<a href="#" style="text-decoration:none">
				<xsl:attribute name="onclick">ShowHideTest(this, 'Ext4FSTestsUnresolved')</xsl:attribute> +</a>
			<div id="Ext4FSTestsUnresolved"  style="border: solid 1px black; padding: 5px; display:none;" class="Unresolved">
				<xsl:for-each select="//Ext4FS[Status='Unresolved']" >
					<xsl:value-of select="@Name" /> : <xsl:value-of select="Operation" /> : <xsl:value-of select="Output" /><br />
				</xsl:for-each>
			</div>
			<br />
			<span class="Unsupported">Total tests unsupported: <xsl:value-of select="count(//Ext4FS[Status='Unsupported'])" /></span>
			<a href="#" style="text-decoration:none">
				<xsl:attribute name="onclick">ShowHideTest(this, 'Ext4FSTestsUnsupported')</xsl:attribute> +</a>
			<div id="Ext4FSTestsUnsupported"  style="border: solid 1px black; padding: 5px; display:none;" class="Unsupported">
				<xsl:for-each select="//Ext4FS[Status='Unsupported']" >
					<xsl:value-of select="@Name" /> : <xsl:value-of select="Operation" /> : <xsl:value-of select="Output" /><br />
				</xsl:for-each>
			</div>
			<br />
						
			
			<!-- JFS -->
			
			<H2>Module: JFS</H2>
			<xsl:for-each select="//JFS[count(. | key('JFSs', @Name)[1]) = 1]">
							
				<xsl:sort select="@Name" />
				
				<xsl:variable name="JFS" select="@Name" />
				<xsl:variable name="TestsTotal" select="count(//JFS[@Name=$JFS])" />
				<xsl:variable name="TestsPassed" select="count(//JFS[@Name=$JFS and Status='Success'])" />
				<span class="Failed">
					<xsl:if test="$TestsPassed=$TestsTotal">
						<xsl:attribute name="class">					
							Success
						</xsl:attribute>
					</xsl:if>
				<xsl:value-of select="$JFS" />			
				(Passed : <xsl:value-of select="$TestsPassed" />/<xsl:value-of select="$TestsTotal" />)
				</span>
				<a href="#" style="text-decoration:none">
				<xsl:attribute name="onclick">ShowHideTest(this, 'JFS_<xsl:value-of select="$JFS" />')</xsl:attribute> +</a>
				<br />
				<div style="border: solid 1px black; padding: 5px; display:none;">	
					<xsl:attribute name="id">JFS_<xsl:value-of select="$JFS" /></xsl:attribute>
					<xsl:for-each select="//JFS[@Name=$JFS]">
						<div>
							<xsl:attribute name="class"><xsl:value-of select="Status" /></xsl:attribute>
							<xsl:value-of select="Operation" />: <xsl:value-of select="Status" /><br />
							<xsl:if test="Status!='Success'">----> Output: <xsl:value-of select="Output" /></xsl:if>
						</div>
					</xsl:for-each>
					<hr />
					Total Tests: <xsl:value-of select="count(//JFS[@Name=$JFS])" /><br />
					<span class="Success">Tests passed: <xsl:value-of select="count(//JFS[@Name=$JFS and Status='Success'])" /></span> <br/>
					<span class="Failed">Tests failed: <xsl:value-of select="count(//JFS[@Name=$JFS and Status='Failed'])" /></span> <br/>
					<span class="Unresolved">Tests unresolved: <xsl:value-of select="count(//JFS[@Name=$JFS and Status='Unresolved'])" /></span> <br/>
					<span class="Unsupported">Tests unsupported: <xsl:value-of select="count(//JFS[@Name=$JFS and Status='Unsupported'])" /></span> <br/>
				</div>
			</xsl:for-each>	
			
			<hr />
			
			<span class="Success">Total tests passed: <xsl:value-of select="count(//JFS[Status='Success'])" /></span> <br/>
			<span class="Failed">Total tests failed: <xsl:value-of select="count(//JFS[Status='Failed'])" /></span>
			<a href="#" style="text-decoration:none">
				<xsl:attribute name="onclick">ShowHideTest(this, 'JFSTestsFailed')</xsl:attribute> +</a>
			<div id="JFSTestsFailed"  style="border: solid 1px black; padding: 5px; display:none;" class="Failed">
				<xsl:for-each select="//JFS[Status='Failed']" >
					<xsl:value-of select="@Name" /> : <xsl:value-of select="Operation" /> : <xsl:value-of select="Output" /><br />
				</xsl:for-each>
			</div>
			<br />
			<span class="Unresolved">Total tests unresolved: <xsl:value-of select="count(//JFS[Status='Unresolved'])" /></span>
			<a href="#" style="text-decoration:none">
				<xsl:attribute name="onclick">ShowHideTest(this, 'JFSTestsUnresolved')</xsl:attribute> +</a>
			<div id="JFSTestsUnresolved"  style="border: solid 1px black; padding: 5px; display:none;" class="Unresolved">
				<xsl:for-each select="//JFS[Status='Unresolved']" >
					<xsl:value-of select="@Name" /> : <xsl:value-of select="Operation" /> : <xsl:value-of select="Output" /><br />
				</xsl:for-each>
			</div>
			<br />
			<span class="Unsupported">Total tests unsupported: <xsl:value-of select="count(//JFS[Status='Unsupported'])" /></span>
			<a href="#" style="text-decoration:none">
				<xsl:attribute name="onclick">ShowHideTest(this, 'JTestsUnsupported')</xsl:attribute> +</a>
			<div id="JFSTestsUnsupported"  style="border: solid 1px black; padding: 5px; display:none;" class="Unsupported">
				<xsl:for-each select="//JFS[Status='Unsupported']" >
					<xsl:value-of select="@Name" /> : <xsl:value-of select="Operation" /> : <xsl:value-of select="Output" /><br />
				</xsl:for-each>
			</div>
			<br />
		
	</body>
</html>
</xsl:template>

</xsl:stylesheet>
