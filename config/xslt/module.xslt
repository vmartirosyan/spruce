<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">



<xsl:template match="//Module">
<div>	
	<xsl:variable name="Module" select="@Name" />
	<H2>Module: <xsl:value-of select="@Name" /></H2>
	<H3>Duration: <xsl:value-of select="//FS/Duration" />s</H3>
	<span>Total tests: <xsl:value-of select="count(//Module[@Name=$Module]/Item)" /></span> <br/>
	<span class="Success">Total tests passed: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='Success'])" /></span> <br/>
	<span class="Shallow">Total shallow tests: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='Shallow'])" /></span> <br/>
	<span class="Fatal">Total tests fatal: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='Fatal'])" /></span>	
	<a href="#" style="text-decoration:none">
		<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Module" />TestsFatal')</xsl:attribute> +</a>
	<div style="border: solid 1px black; padding: 5px; display:none;" class="Fatal">
		<xsl:attribute name="Id"><xsl:value-of select="$Module" />TestsFatal</xsl:attribute>
		<table width="100%">
			<tr>
				<th width="20%">Name</th>
				<th width="20%">Operation</th>
				<th width="60%">Output</th>
			</tr>
		<xsl:for-each select="//Module[@Name=$Module]/Item[Status='Fatal']" >				
			<tr>
				<td width="20%"><xsl:value-of select="@Name" /></td>
				<td width="20%"><xsl:value-of select="Operation" /></td>
				<td width="60%"><pre><xsl:value-of select="Output" /></pre></td>
			</tr>
		</xsl:for-each>
		</table>
	</div>
	<br />
	<span class="Signaled">Total tests signalled: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='Signaled'])" /></span>	
	<a href="#" style="text-decoration:none">
		<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Module" />TestsSignaled')</xsl:attribute> +</a>
	<div style="border: solid 1px black; padding: 5px; display:none;" class="Signaled">
		<xsl:attribute name="Id"><xsl:value-of select="$Module" />TestsSignaled</xsl:attribute>
		<table width="100%">
			<tr>
				<th width="20%">Name</th>
				<th width="20%">Operation</th>
				<th width="60%">Output</th>
			</tr>
		<xsl:for-each select="//Module[@Name=$Module]/Item[Status='Signaled']" >				
			<tr>
				<td width="20%"><xsl:value-of select="@Name" /></td>
				<td width="20%"><xsl:value-of select="Operation" /></td>
				<td width="60%"><pre><xsl:value-of select="Output" /></pre></td>
			</tr>
		</xsl:for-each>
		</table>
	</div>
	<br />
	<span class="Failed">Total tests failed: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='Failed'])" /></span>
	<a href="#" style="text-decoration:none">
		<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Module" />TestsFailed')</xsl:attribute> +</a>
	<div style="border: solid 1px black; padding: 5px; display:none;" class="Failed">
		<xsl:attribute name="Id"><xsl:value-of select="$Module" />TestsFailed</xsl:attribute>
		<table width="100%">
			<tr>
				<th width="20%">Name</th>
				<th width="20%">Operation</th>
				<th width="60%">Output</th>
			</tr>
		<xsl:for-each select="//Module[@Name=$Module]/Item[Status='Failed']" >				
			<tr>
				<td width="20%"><xsl:value-of select="@Name" /></td>
				<td width="20%"><xsl:value-of select="Operation" /></td>
				<td width="60%"><pre><xsl:value-of select="Output" /></pre></td>
			</tr>
		</xsl:for-each>
		</table>
	</div>
	<br />
	<span class="FSimSuccess">Total fault simulation tests failed: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='FSimSuccess'])" /></span>
	<a href="#" style="text-decoration:none">
		<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Module" />TestsFaultSimFailed')</xsl:attribute> +</a>
	<div style="border: solid 1px black; padding: 5px; display:none;" class="FSimSuccess">
		<xsl:attribute name="Id"><xsl:value-of select="$Module" />TestsFaultSimFailed</xsl:attribute>
		<table width="100%">
			<tr>
				<th width="20%">Name</th>
				<th width="20%">Operation</th>
				<th width="60%">Output</th>
			</tr>
		<xsl:for-each select="//Module[@Name=$Module]/Item[Status='FSimSuccess']" >				
			<tr>
				<td width="20%"><xsl:value-of select="@Name" /></td>
				<td width="20%"><xsl:value-of select="Operation" /></td>
				<td width="60%"><pre><xsl:value-of select="Output" /></pre></td>
			</tr>
		</xsl:for-each>
		</table>
	</div>
	<br />
	<span class="FSimFail">Total fault simulation tests passed: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='FSimFail'])" /></span>
	<a href="#" style="text-decoration:none">
		<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Module" />TestsFaultSimPassed')</xsl:attribute> +</a>
	<div style="border: solid 1px black; padding: 5px; display:none;" class="FSimFail">
		<xsl:attribute name="Id"><xsl:value-of select="$Module" />TestsFaultSimPassed</xsl:attribute>
		<table width="100%">
			<tr>
				<th width="20%">Name</th>
				<th width="20%">Operation</th>
				<th width="60%">Output</th>
			</tr>
		<xsl:for-each select="//Module[@Name=$Module]/Item[Status='FSimFail']" >				
			<tr>
				<td width="20%"><xsl:value-of select="@Name" /></td>
				<td width="20%"><xsl:value-of select="Operation" /></td>
				<td width="60%"><pre><xsl:value-of select="Output" /></pre></td>
			</tr>
		</xsl:for-each>
		</table>
	</div>
	<br />
	<span class="Unresolved">Total tests unresolved: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='Unresolved'])" /></span>
	<a href="#" style="text-decoration:none">
		<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Module" />TestsUnresolved')</xsl:attribute> +</a>
	<div style="border: solid 1px black; padding: 5px; display:none;" class="Unresolved">
		<xsl:attribute name="Id"><xsl:value-of select="$Module" />TestsUnresolved</xsl:attribute>
		<table width="100%">
			<tr>
				<th width="20%">Name</th>
				<th width="20%">Operation</th>
				<th width="60%">Output</th>
			</tr>
		<xsl:for-each select="//Module[@Name=$Module]/Item[Status='Unresolved']" >				
			<tr>
				<td width="20%"><xsl:value-of select="@Name" /></td>
				<td width="20%"><xsl:value-of select="Operation" /></td>
				<td width="60%"><pre><xsl:value-of select="Output" /></pre></td>
			</tr>
		</xsl:for-each>
		</table>
	</div>
	<br />
	<span class="Unsupported">Total tests unsupported: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='Unsupported'])" /></span>
	<a href="#" style="text-decoration:none">
		<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Module" />TestsUnsupported')</xsl:attribute> +</a>
	<div style="border: solid 1px black; padding: 5px; display:none;" class="Unsupported">
		<xsl:attribute name="Id"><xsl:value-of select="$Module" />TestsUnsupported</xsl:attribute>
		<table width="100%">
			<tr>
				<th width="20%">Name</th>
				<th width="20%">Operation</th>
				<th width="60%">Output</th>
			</tr>
		<xsl:for-each select="//Module[@Name=$Module]/Item[Status='Unsupported']" >				
			<tr>
				<td width="20%"><xsl:value-of select="@Name" /></td>
				<td width="20%"><xsl:value-of select="Operation" /></td>
				<td width="60%"><pre><xsl:value-of select="Output" /></pre></td>
			</tr>
		</xsl:for-each>
		</table>
	</div>
	<br />
	<span class="Skipped">Total tests skipped: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='Skipped'])" /></span>
	<a href="#" style="text-decoration:none">
		<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Module" />TestsSkipped')</xsl:attribute> +</a>
	<div style="border: solid 1px black; padding: 5px; display:none;" class="Skipped">
		<xsl:attribute name="Id"><xsl:value-of select="$Module" />TestsSkipped</xsl:attribute>
		<table width="100%">
			<tr>
				<th width="20%">Name</th>
				<th width="20%">Operation</th>
				<th width="60%">Output</th>
			</tr>
		<xsl:for-each select="//Module[@Name=$Module]/Item[Status='Skipped']" >				
			<tr>
				<td width="20%"><xsl:value-of select="@Name" /></td>
				<td width="20%"><xsl:value-of select="Operation" /></td>
				<td width="60%"><pre><xsl:value-of select="Output" /></pre></td>
			</tr>
		</xsl:for-each>
		</table>
	</div>
	<br />
	<span class="Timeout">Total tests timed out: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='Timeout'])" /></span>
	<a href="#" style="text-decoration:none">
		<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Module" />TestsTimedOut')</xsl:attribute> +</a>
	<div style="border: solid 1px black; padding: 5px; display:none;" class="Timeout">
		<xsl:attribute name="Id"><xsl:value-of select="$Module" />TestsTimedOut</xsl:attribute>
		<table width="100%">
			<tr>
				<th width="20%">Name</th>
				<th width="20%">Operation</th>
				<th width="60%">Output</th>
			</tr>
		<xsl:for-each select="//Module[@Name=$Module]/Item[Status='Timeout']" >				
			<tr>
				<td width="20%"><xsl:value-of select="@Name" /></td>
				<td width="20%"><xsl:value-of select="Operation" /></td>
				<td width="60%"><pre><xsl:value-of select="Output" /></pre></td>
			</tr>
		</xsl:for-each>
		</table>
	</div>			
	<br /><br />
	
	<h2>Details</h2>
	
	<xsl:variable name="key_name"><xsl:value-of select="$Module" />s</xsl:variable>
	<!--count(. | key($key_name, @Name)[1]) = 1]" -->
	<xsl:for-each select="//Module[@Name=$Module]/Item[not(@Name=preceding-sibling::Item/@Name)]" >
		
		<xsl:sort select="@Name" />
		
		<xsl:variable name="Item" select="@Name" />
		<xsl:variable name="TestsTotal" select="count(//Module[@Name=$Module]/Item[@Name=$Item])" />
		<xsl:variable name="TestsPassed" select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Success']) + count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Shallow'])" />
		<span class="Success">			
			<xsl:if test="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Timeout'])">
				<xsl:attribute name="class">Timeout</xsl:attribute>
			</xsl:if>
			<xsl:if test="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Unsupported'])">
				<xsl:attribute name="class">Unsupported</xsl:attribute>
			</xsl:if>
			<xsl:if test="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Unresolved'])">
				<xsl:attribute name="class">Unresolved</xsl:attribute>
			</xsl:if>
			<xsl:if test="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Failed'])">
				<xsl:attribute name="class">Failed</xsl:attribute>
			</xsl:if>
			<xsl:if test="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Fatal'])">
				<xsl:attribute name="class">Fatal</xsl:attribute>
			</xsl:if>
			<xsl:choose>
				<xsl:when test="$TestsTotal != $TestsPassed">
					<a href="#" style="text-decoration:none">
						<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Module" />_<xsl:value-of select="$Item" />')</xsl:attribute> +</a>
				</xsl:when>
				<xsl:otherwise>&#160;&#160;&#160;</xsl:otherwise>
			</xsl:choose>
			
		<xsl:value-of select="$Item" />
		(Passed : <xsl:value-of select="$TestsPassed" />/<xsl:value-of select="$TestsTotal" />)
		</span>		
		<br />
		<div style="border: solid 1px black; padding: 5px; display:none;">	
			<xsl:attribute name="id"><xsl:value-of select="$Module" />_<xsl:value-of select="$Item" /></xsl:attribute>
			Total Tests: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item])" /><br />
			<span class="Success">Tests passed: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Success'])" /></span> <br/>
			<span class="Shallow">Shallow tests: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Shallow'])" /></span> <br/>
			<span class="Failed">Tests failed: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Failed'])" /></span> <br/>
			<span class="Signaled">Tests signaled: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Signaled'])" /></span> <br/>
			<span class="Unresolved">Tests unresolved: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Unresolved'])" /></span> <br/>
			<span class="FSimSuccess">Tests fault_sim failed: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='FSimSuccess'])" /></span> <br/>
			<span class="FSimFail">Tests fault_sim passed: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='FSimFail'])" /></span> <br/>
			<span class="Unsupported">Tests unsupported: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Unsupported'])" /></span> <br/>
			<span class="Timeout">Tests timed out: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Timeout'])" /></span> <br/>
			<span class="Fatal">Tests fatal: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Fatal'])" /></span> <br/>
			<hr />
			<table width="100%">
				<tr>
					<th width="20%">Name</th>
					<th width="20%">Operation</th>
					<th width="60%">Output</th>
				</tr>
				<xsl:for-each select="//Module[@Name=$Module]/Item[@Name=$Item]">
					<xsl:if test="Status!='Success' and Status!='Shallow'">
						<tr>
							<xsl:attribute name="class"><xsl:value-of select="Status" /></xsl:attribute>
							<td width="20%"><xsl:value-of select="@Name" /></td>
							<td width="20%"><xsl:value-of select="Operation" /></td>
							<td width="60%"><pre><xsl:value-of select="Output" /></pre></td>
						</tr>
					</xsl:if>
				</xsl:for-each>
			</table>
		</div>
	</xsl:for-each>	
</div>
</xsl:template>

</xsl:stylesheet>
