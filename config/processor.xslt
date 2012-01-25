<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/TestResults">
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
		<title>Test log</title>
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
		<xsl:for-each select="//SysCall[not(.=../preceding::SysCall)]">
			<xsl:variable name="SysCall"><xsl:value-of select="."/></xsl:variable>
			<xsl:variable name="TestsTotal" select="count(//TestResult[SysCall=$SysCall])" />
			<xsl:variable name="TestsPassed" select="count(//TestResult[SysCall=$SysCall and Status='Success'])" />
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
			<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$SysCall" />')</xsl:attribute> +</a>
			<br />
			<div style="border: solid 1px black; padding: 5px; display:none;">	
				<xsl:attribute name="id"><xsl:value-of select="$SysCall" /></xsl:attribute>
				<xsl:for-each select="//TestResult[SysCall=$SysCall]">
					<div>
						<xsl:attribute name="class"><xsl:value-of select="Status" /></xsl:attribute>
						<xsl:value-of select="Operation" />: <xsl:value-of select="Status" />
						<xsl:if test="Status!='Success'"><xsl:value-of select="Output" /></xsl:if>
					</div>
				</xsl:for-each>
				Total Tests: <xsl:value-of select="count(//TestResult[SysCall=$SysCall])" /><br />
				<span class="Success">Tests passed: <xsl:value-of select="count(//TestResult[SysCall=$SysCall and Status='Success'])" /></span> <br/>
				<span class="Failed">Tests failed: <xsl:value-of select="count(//TestResult[SysCall=$SysCall and Status='Failed'])" /></span> <br/>
				<span class="Unresolved">Tests unresolved: <xsl:value-of select="count(//TestResult[SysCall=$SysCall and Status='Unresolved'])" /></span> <br/>
				<span class="Unsupported">Tests unsupported: <xsl:value-of select="count(//TestResult[SysCall=$SysCall and Status='Unsupported'])" /></span> <br/>
			</div>
		</xsl:for-each>	
		
		<hr />
		
		<span class="Success">Total tests passed: <xsl:value-of select="count(//TestResult[Status='Success'])" /></span> <br/>
		<span class="Failed">Total tests failed: <xsl:value-of select="count(//TestResult[Status='Failed'])" /></span>
		<a href="#" style="text-decoration:none">
			<xsl:attribute name="onclick">ShowHideTest(this, 'TestsFailed')</xsl:attribute> +</a>
		<div id="TestsFailed"  style="border: solid 1px black; padding: 5px; display:none;" class="Failed">
			<xsl:for-each select="//TestResult[Status='Failed']" >
				<xsl:value-of select="SysCall" /> : <xsl:value-of select="Operation" /> : <xsl:value-of select="Output" /><br />
			</xsl:for-each>
		</div>
		<br />
		<span class="Unresolved">Total tests unresolved: <xsl:value-of select="count(//TestResult[Status='Unresolved'])" /></span>
		<a href="#" style="text-decoration:none">
			<xsl:attribute name="onclick">ShowHideTest(this, 'TestsUnresolved')</xsl:attribute> +</a>
		<div id="TestsUnresolved"  style="border: solid 1px black; padding: 5px; display:none;" class="Unresolved">
			<xsl:for-each select="//TestResult[Status='Unresolved']" >
				<xsl:value-of select="SysCall" /> : <xsl:value-of select="Operation" /> : <xsl:value-of select="Output" /><br />
			</xsl:for-each>
		</div>
		<br />
		<span class="Unsupported">Total tests unresolved: <xsl:value-of select="count(//TestResult[Status='Unsupported'])" /></span>
		<a href="#" style="text-decoration:none">
			<xsl:attribute name="onclick">ShowHideTest(this, 'TestsUnsupported')</xsl:attribute> +</a>
		<div id="TestsUnsupported"  style="border: solid 1px black; padding: 5px; display:none;" class="Unsupported">
			<xsl:for-each select="//TestResult[Status='Unsupported']" >
				<xsl:value-of select="SysCall" /> : <xsl:value-of select="Operation" /> : <xsl:value-of select="Output" /><br />
			</xsl:for-each>
		</div>
		<br />
	</body>
</html>
</xsl:template>

</xsl:stylesheet>
