<?xml version="1.0" encoding="utf-8"?>
<!--    testset.xslt
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Authors:
//          Ruzanna Karakozova <r.karakozova@gmail.com>
//          Vahram Martirosyan <vmartirosyan@gmail.com>
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
#include &lt;memory>
#include &lt;map>
#include &lt;Test.hpp>
#include &lt;Logger.hpp>
#include &lt;PartitionManager.hpp>
#include &lt;KedrIntegrator.hpp>
#include &lt;File.hpp>
#include &lt;Directory.hpp>
#include &lt;linux/version.h>
using std::map;
using std::string;

/* 
 * Macros for use in tests and testsets before user-defined code is executed.
 * This macros do not execute 'footer' part.
 */
#define Skip_direct(cond, message)\
    if (cond) \
        {Logger::LogWarn(message); return Skipped; }

#define Unres_direct(cond, message)\
    if (cond) \
        {Logger::LogError(message); return Unresolved; }
    
#define Unsupp_direct(message)\
    {Logger::LogWarn(message); return Unsupported; }

#define Fatal_direct(message)\
    {Logger::LogError(message); return Fatal; }

    
    <xsl:for-each select="Requires">
#include &lt;<xsl:value-of select="." />>
    </xsl:for-each> 

<!-- Check if all the requirements are satisfied -->
    <xsl:if test="RequiresAll">
<!-- Use standart 'if(false)' construction for template-generated alternatives -->
#if 0
        <xsl:if test="RequiresAll/@KernelVersion!=''">
#elif LINUX_VERSION_CODE &lt; <xsl:call-template name="kernel-version-value">
        <xsl:with-param name="version" select="RequiresAll/@KernelVersion"/>
            </xsl:call-template>
    EmptyTestSet(<xsl:value-of select="$PackageName" />, <xsl:value-of select="/TestSet/@Name" />, Unsupported, "Testset <xsl:value-of select="/TestSet/@Name" /> is not supported. Kernel version should be at least <xsl:value-of select="RequiresAll/@KernelVersion" />.");
        </xsl:if>
        <xsl:if test="RequiresAll/@Defined!=''">
#elif ! defined <xsl:value-of select="RequiresAll/@Defined" />
    EmptyTestSet(<xsl:value-of select="$PackageName" />, <xsl:value-of select="/TestSet/@Name" />, Unsupported, "Testset <xsl:value-of select="/TestSet/@Name" /> is not supported. Required macro <xsl:value-of select="RequiresAll/@Defined" /> is not defined.");
        </xsl:if>            
        <xsl:if test="RequiresAll/@Condition!=''">
#elif ! (<xsl:value-of select="RequiresAll/@Condition" />)
    EmptyTestSet(<xsl:value-of select="$PackageName" />, <xsl:value-of select="/TestSet/@Name" />, Unsupported, "Testset <xsl:value-of select="/TestSet/@Name" /> is not supported. Required condition '<xsl:value-of select="RequiresAll/@Condition" />' is not satisfied.");
        </xsl:if>            
#else
    </xsl:if>

extern char * DeviceName;
extern char * MountPoint;
extern char * FileSystem;

extern bool terminate_process;

<xsl:value-of select="GlobalHeader"/>

<xsl:variable name="TestSetName" select="@Name" />
    
<xsl:value-of select="/TestSet/Internal"/>
    
    
    <xsl:for-each select="Test">
int <xsl:value-of select="$PackageName" />_<xsl:value-of select="$TestSetName" />_<xsl:value-of select="@Name" />Func(Process const * param, vector&lt;string>)
{
<!-- Check if all the requirements are satisfied -->
    <xsl:if test="Requires">
<!-- Use standart 'if(false)' construction for template-generated alternatives -->
#if 0
        <xsl:if test="Requires/@KernelVersion!=''">
#elif LINUX_VERSION_CODE &lt; <xsl:call-template name="kernel-version-value">
        <xsl:with-param name="version" select="Requires/@KernelVersion"/>
            </xsl:call-template>
    Unsupp_direct("Operation is not supported. Kernel version should be at least <xsl:value-of select="Requires/@KernelVersion" />.");
        </xsl:if>
        <xsl:if test="Requires/@Defined!=''">
#elif ! defined <xsl:value-of select="Requires/@Defined" />
    Unsupp_direct("Required macro <xsl:value-of select="Requires/@Defined" /> is not defined.");
        </xsl:if>            
        <xsl:if test="Requires/@Condition!=''">
#elif ! (<xsl:value-of select="Requires/@Condition" />)
    Unsupp_direct("Required condition '<xsl:value-of select="Requires/@Condition" />' is not satisfied.");
        </xsl:if>            
#else
    </xsl:if>

    <xsl:if test="@per-mount-options='false'">
    Skip_direct ( !PartitionManager::NoOptionsEnabled(), "Test <xsl:value-of select="@Name" /> is executed only when no options are provided.")
    </xsl:if>

    <xsl:for-each select="../RunIf">
    Skip_direct(!(<xsl:call-template name="trim"><xsl:with-param name="text" select="."/></xsl:call-template>),
        "<xsl:choose><xsl:when test="@message"><xsl:value-of select="@message"/></xsl:when><xsl:otherwise>Testset is not run in that conditions.</xsl:otherwise></xsl:choose>");
    </xsl:for-each>

    <xsl:for-each select="RunIf">
    Skip_direct(!(<xsl:call-template name="trim"><xsl:with-param name="text" select="."/></xsl:call-template>),
        "<xsl:choose><xsl:when test="@message"><xsl:value-of select="@message"/></xsl:when><xsl:otherwise>Test is not run in that conditions.</xsl:otherwise></xsl:choose>");
    </xsl:for-each>
    
    Test const * obj = dynamic_cast&lt;Test const*>(param);
    // Use the obj to prevent unused variable warning
    obj = obj;

    <!-- Simulate 'Dangerous' behaviour -->
    <xsl:if test="Dangerous">if((obj->GetSupportedChecks() &amp; Dangerous) &amp;&amp; !(obj->GetEffectiveChecks() &amp; Dangerous))
    {
    <xsl:for-each select="Dangerous">
        Skip_direct(1 <xsl:if test="@check">
                &amp;&amp; <xsl:call-template name="is-check"><xsl:with-param name="text" select="@check"/></xsl:call-template></xsl:if><xsl:if test="@fs">
                &amp;&amp; !strcmp(FileSystem, "<xsl:value-of select="@fs"/>")</xsl:if><xsl:if test="@since-kernel">
                &amp;&amp; (LINUX_VERSION_CODE &gt;= <xsl:call-template name="kernel-version-value"><xsl:with-param name="version" select="@since-kernel"/></xsl:call-template>)</xsl:if><xsl:if test="@until-kernel">
                &amp;&amp; (LINUX_VERSION_CODE &lt; <xsl:call-template name="kernel-version-value"><xsl:with-param name="version" select="@until-kernel"/></xsl:call-template>)</xsl:if><xsl:if test="@mount_opt">
                &amp;&amp; PartitionManager::IsOptionEnabled("<xsl:value-of select="@mount_opt"/>")</xsl:if>,
                "Test is skipped as dangerous. For execute it add 'dangerous' to 'checks' parameter.");
    </xsl:for-each>
    }
    </xsl:if>

    <xsl:if test="@mount-oriented='false' or /TestSet/@mount-oriented='false'">
    <!-- Test is not a mount-oriented. Unmount filesystem before test execution as it contain effects of previous tests. -->
    if( !PartitionManager::ReleasePartition(MountPoint))
        Fatal_direct ("Failed to umount partition before non-mount-oriented test");

        <xsl:if test="@use-mount='true' or /TestSet/@use-mount='true' or @use-mkfs='true' or /TestSet/@use-mkfs='false'">
        <!-- Test use clean formatted device. -->
    if( !PartitionManager::CreateFilesystem(FileSystem, DeviceName, false, getenv("MkfsOpts")))
        Fatal_direct ("Failed to prepare clean partition for the test.");
            <xsl:if test="@use-mount='true' or /TestSet/@use-mount='true'">
            <!-- Test use clean mounted device. -->
    if( !PartitionManager::Mount(DeviceName,MountPoint,FileSystem,getenv("MountData"), atoi(getenv("MountFlags"))))
        Fatal_direct ("Failed to mount clean partition for the test.");
    Unres_direct( chdir(MountPoint) == -1, "Failed to change directory to the mounted device.");
            </xsl:if>
        </xsl:if>
    </xsl:if>

    <xsl:if test="Dev">
    const int DevCount = <xsl:choose><xsl:when test="Dev/@count"><xsl:value-of select="Dev/@count"/></xsl:when><xsl:otherwise>1</xsl:otherwise></xsl:choose>;
    string DevPaths[DevCount];
    {
        <!-- Check that additional devices are available and prepare them for use. -->
        if(KedrIntegrator::DoesModuleExist("brd"))
        {
            Unsupp_direct("To run this test rebuild the kernel with CONFIG_BLK_DEV_RAM:=y configuration option.");
        }
        UnixCommand modprobe("modprobe");
        vector&lt;string&gt; args(1, "drm");
        ProcessResult * res = modprobe.Execute(args);
        Unres_direct(res->GetStatus() != Success, "Cannot load \"drm\" module into kernel: " + res->GetOutput() );
        delete res;
    }
    for(int i = 0; i &lt; DevCount; i++)
    {
        ostringstream devname;
        
        devname &lt;&lt; "/dev/ram" &lt;&lt; i;
        DevPaths[i] = devname.str();
    }
    </xsl:if>

    <!-- Generate the test function -->
    Status _TestStatus = <xsl:choose><xsl:when test="@Shallow='true'" >Shallow</xsl:when><xsl:otherwise>Success</xsl:otherwise></xsl:choose>;
    bool _InFooter = false;    
        
    <xsl:value-of select="/TestSet/Header" />
    
    <xsl:value-of select="Header" />

    try
    {
        string DirPrefix = "<xsl:value-of select="$TestSetName" />_<xsl:value-of select="@Name"/>_dir_";
        string FilePrefix = "<xsl:value-of select="$TestSetName" />_<xsl:value-of select="@Name"/>_file_";
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
        <xsl:if test="Dir/Dir">
        const int DirDirCount = <xsl:value-of select="Dir/Dir/@count"/>;
        string DirDirPaths[DirDirCount];
        Directory DirDirs[DirDirCount];
        int DirDDs[DirDirCount];
        </xsl:if>

        for ( int i = 0 ; i &lt; DirCount; ++i )
        {
            const int size = 10;
            char buf[size];
            Unres ( snprintf(buf, size, "%d", i) >= size, "Cannot prepare DirPath value.");
            
            DirPaths[i] = DirPrefix + buf;
            DirDs[i] = Dirs[i].Open(DirPaths[i], S_IRWXU);
            if ( DirDs[i] == -1 )
            {
                throw Exception("Directory descriptor is not valid.");
            }
        }
        
        <xsl:if test="Dir/File">
        for ( int i = 0 ; i &lt; DirFileCount; ++i )
        {
            const int size = 10;
            char buf[size];
            Unres ( snprintf(buf, size, "%d", i) >= size, "Cannot prepare DirFilePath value.");
            
            DirFilePaths[i] = DirPaths[0] + "/" + FilePrefix + buf;
            DirFDs[i] = DirFiles[i].Open(DirFilePaths[i], S_IRWXU, O_CREAT | O_RDWR);
            if ( DirFDs[i] == -1 )
            {
                throw Exception("File descriptor is not valid.");
            }
        }    
        </xsl:if>
        
        <xsl:if test="Dir/Dir">
        for ( int i = 0 ; i &lt; DirDirCount; ++i )
        {
            const int size = 10;
            char buf[size];
            Unres ( snprintf(buf, size, "%d", i) >= size, "Cannot prepare DirDirPath value.");
            
            DirDirPaths[i] = DirPaths[0] + "/" + DirPrefix + buf;
            DirDDs[i] = DirDirs[i].Open(DirDirPaths[i], S_IRWXU);
            if ( DirDDs[i] == -1 )
            {
                throw Exception("Dir descriptor is not valid.");
            }
        }    
        </xsl:if>
        
        </xsl:if>
        <xsl:if test="File">
        const int FileCount = <xsl:value-of select="File/@count"/>;
        int FileFlags = <xsl:choose><xsl:when test="File/@flags != ''"><xsl:value-of select="File/@flags"/></xsl:when><xsl:otherwise>O_CREAT | O_RDWR</xsl:otherwise></xsl:choose>;
        int FileMode = <xsl:choose><xsl:when test="File/@mode != ''"><xsl:value-of select="File/@mode"/></xsl:when><xsl:otherwise>S_IRWXU</xsl:otherwise></xsl:choose>;
        
        string FilePaths[FileCount];
        File Files[FileCount];
        int FDs[FileCount];
        for ( int i = 0 ; i &lt; FileCount; ++i )
        {
            const int size = 10;
            char buf[size];
            Unres ( snprintf(buf, size, "%d", i) >= size, "Cannot prepare FilePath value.");
            
            FilePaths[i] = FilePrefix + buf;
            FDs[i] = Files[i].Open(FilePaths[i], FileMode, FileFlags);
            if ( FDs[i] == -1 )
            {
                throw Exception("File descriptor is not valid.");
            }
        }    
        </xsl:if>
        
        <xsl:value-of select="Code" />
    }
    catch (Exception e)
    {
        if ( PartitionManager::IsOptionEnabled("ro") &amp;&amp; e.GetErrno() == EROFS )
            Unsupp("Read only file system.");
        
        Error("Exception was thrown: ", e.GetMessage(), Unresolved);
    }

Footer: 
    _InFooter = true;

    <xsl:value-of select="Footer" />
    <xsl:value-of select="/TestSet/Footer" />    

    <xsl:if test="@mount-oriented='false' or /TestSet/@mount-oriented='false'">
    <!-- Test is not a mount-oriented. Restore filesystem after test execution. -->
    if( PartitionManager::RestorePartition(DeviceName, MountPoint, FileSystem, true)!= PS_Success )
    {
        Error("Failed to restore partition after non-mount-oriented test");
        _exit(Fatal);
    }
    </xsl:if>

    return _TestStatus;

    <xsl:if test="Requires">
<!-- Terminate #if directive. -->
#endif /* Requires */
    </xsl:if>
}

</xsl:for-each>

<xsl:if test="/TestSet/StartUp!='' or /TestSet/@per-mount-options='false'">
Status <xsl:value-of select="$PackageName" />_<xsl:value-of select="$TestSetName" />StartUp()
{
    bool _InFooter = true;
    Status _TestStatus = Success;

    <xsl:if test="/TestSet/StartUp!=''">
    <xsl:value-of select="/TestSet/StartUp" />
    </xsl:if>
    <xsl:if test="/TestSet/@per-mount-options='false'">
    <!--Test set is not per-mount-options -->
    Skip ( !PartitionManager::NoOptionsEnabled(), "Testset <xsl:value-of select="$TestSetName" /> is executed only when no options are provided." );
    </xsl:if>
Footer:
    return _TestStatus;
}
    
</xsl:if>

<xsl:if test="/TestSet/CleanUp!=''">
void <xsl:value-of select="$PackageName" />_<xsl:value-of select="$TestSetName" />CleanUp()
{
    <xsl:value-of select="/TestSet/CleanUp" />
}
</xsl:if>

TestSet Init_<xsl:value-of select="$PackageName" />_<xsl:value-of select="/TestSet/@Name" />()
{

    TestSet <xsl:value-of select="$TestSetName" />Tests("<xsl:value-of select="/TestSet/@Name" />");
    
    Checks check;
    
    <xsl:for-each select="Test">
        
    check = All;
    
    <xsl:if test="@NonFunctional='true'">
        check = check &amp; ~Functional;
    </xsl:if>
    
    <!-- If test is dangerous in some sence, need to set corresponded flag. Otherwise GetEffectiveChecks() won't include this flag. -->
    <xsl:if test="Dangerous">
        check = check | Dangerous;
    </xsl:if>
    
    <xsl:value-of select="$TestSetName" />Tests.AddTest(Test(
        "<xsl:value-of select="@Name" />",
        "<xsl:value-of select="Description" />",
        <xsl:value-of select="$PackageName" />_<xsl:value-of select="$TestSetName" />_<xsl:value-of select="@Name" />Func,
        check
        ));
    
    </xsl:for-each>
    
    <xsl:if test="/TestSet/StartUp!='' or /TestSet/@per-mount-options='false'">
        <xsl:value-of select="$TestSetName" />Tests.SetStartUpFunc(<xsl:value-of select="$PackageName" />_<xsl:value-of select="$TestSetName" />StartUp);
    </xsl:if>
    <xsl:if test="/TestSet/CleanUp!=''">
        <xsl:value-of select="$TestSetName" />Tests.SetCleanUpFunc(<xsl:value-of select="$PackageName" />_<xsl:value-of select="$TestSetName" />CleanUp);
    </xsl:if>
    
    return <xsl:value-of select="$TestSetName" />Tests;
}

<xsl:value-of select="GlobalFooter"/>

<xsl:if test="RequiresAll">
<!-- Terminate #if directive. -->
#endif /* RequiresAll */
</xsl:if>

    </xsl:template>
    
<xsl:template name="string-replace-all">
  <xsl:param name="text"/>
  <xsl:param name="replace"/>
  <xsl:param name="by"/>
  <xsl:choose>
    <xsl:when test="contains($text,$replace)">
      <xsl:value-of select="substring-before($text,$replace)"/>
      <xsl:value-of select="$by"/>
      <xsl:call-template name="string-replace-all">
        <xsl:with-param name="text" select="substring-after($text,$replace)"/>
        <xsl:with-param name="replace" select="$replace"/>
        <xsl:with-param name="by" select="$by"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$text"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>



<xsl:template name="ltrim">
  <xsl:param name="text"/>
  <xsl:variable name="spaceChars" select="' &#9;&#10;&#13;'"/>
  <xsl:variable name="nonWs" select="translate($text, $spaceChars, '')"/>
  <xsl:if test="$nonWs">
    <xsl:variable name="firstNonWs" select="substring($nonWs, 1, 1)"/>
    <xsl:value-of select="concat($firstNonWs, substring-after($text, $firstNonWs))"/>
  </xsl:if>
</xsl:template>

<xsl:template name="rtrim">
  <xsl:param name="text"/>
  <xsl:variable name="len" select="string-length($text)"/>
  <xsl:variable name="lastChar" select="substring($text, $len)"/>
  <xsl:choose>
  <xsl:when test="not($lastChar)"/>
  <xsl:when test="normalize-space($lastChar)">
    <xsl:value-of select="$text"/>
  </xsl:when>
  <xsl:otherwise>
    <xsl:call-template name="rtrim">
        <xsl:with-param name="text" select="substring($text, 1, $len - 1)"/>
    </xsl:call-template>
  </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<xsl:template name="trim">
  <xsl:param name="text"/>
  <xsl:call-template name="rtrim">
    <xsl:with-param name="text">
        <xsl:call-template name="ltrim">
            <xsl:with-param name="text" select="$text"/>
        </xsl:call-template>
    </xsl:with-param>
  </xsl:call-template>
</xsl:template>


<xsl:template name="kernel-version-value">
  <xsl:param name="version"/>KERNEL_VERSION(<xsl:call-template name="string-replace-all">
        <xsl:with-param name="text" select="$version"/>
        <xsl:with-param name="replace" select="'.'"/>
        <xsl:with-param name="by" select="','"/>
    </xsl:call-template>)<!--Strip unwanted newline using that comment-->
</xsl:template>


<!--
    Check, whether test is currently executed for given check.
    
    NOTE: For execution with Fault Simulation enabled we distinguish cases
    when collect information for future simulations and when fault are
    really simulated. Only second case match 'stability' check.
    
    Note also, that test is not run in stability mode if it is not run in basic mode(func).
-->
<xsl:template name="is-check">
  <xsl:param name="text"/>
  <xsl:choose>
    <xsl:when test="$text='stability'">obj->GetEffectiveChecks() == Stability</xsl:when>
    <xsl:when test="$text='func'">obj->GetEffectiveChecks() &amp; Functional</xsl:when>
    <xsl:when test="$text='mem-leak'">obj->GetEffectiveChecks() &amp; MemoryLeak</xsl:when>
    <!-- 'dangerous' is not really a check, it is a flag affected on set of tests. -->
    <xsl:message terminate="on">Incorrect value of check filtered: <xsl:value-of select="$text"/></xsl:message>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
