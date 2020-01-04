from xml.etree.ElementTree import Element, SubElement
from xml.etree import ElementTree
import xml.dom.minidom
import os
import zipfile
import re

VERSION = "666"

with open('goldboard/config.h') as f:
	for line in f.readlines():
		if "VERSION" in line:
			version = str(int(line.split("VERSION")[-1]))
			VERSION = version[0] + "." + version[1:]

EXCLUDED_FILES = ["*.zip","*.py",".gitignore",".yml","Makefile"]
TEMPLATE_NAME = "goldboard4-V"+VERSION+"-template"
DEFAULT_NAME = "goldboard4-V"+VERSION+"-project"
DESCRIPTION = "Lib for goldboard 4.0"
TEMPLATE_ICON = "__TemplateIcon.ico"
PREVIEW_IMAGE = "__PreviewImage.png"

def check_if_file_is_excluded(filename):
	filename_without_directory = filename.split("\\")[-1].split("/")[-1]
	if filename_without_directory in EXCLUDED_FILES:
		return True
	for excluded_name in EXCLUDED_FILES:
		if excluded_name[0] == "*":
			if excluded_name[1:] in filename_without_directory:
				return True
	return False

def generate_vs_template_content():
	vstemplate = Element("VSTemplate")
	vstemplate.set("Version", "3.0.0")
	vstemplate.set(
		"xmlns", "http://schemas.microsoft.com/developer/vstemplate/2005")
	vstemplate.set("Type", "Project")
	template_data = SubElement(vstemplate, "TemplateData")
	SubElement(template_data, "Name").text = TEMPLATE_NAME
	SubElement(template_data, "Description").text = DESCRIPTION
	SubElement(template_data, "ProjectType").text = "CandCPP"
	SubElement(template_data, "ProjectSubType").text = ""
	SubElement(template_data, "SortOrder").text = "1000"
	SubElement(template_data, "CreateNewFolder").text = "true"
	SubElement(template_data, "DefaultName").text = DEFAULT_NAME
	SubElement(template_data, "ProvideDefaultName").text = "true"
	SubElement(template_data, "LocationField").text = "Enabled"
	SubElement(template_data, "EnableLocationBrowseButton").text = "true"
	SubElement(template_data, "Icon").text = TEMPLATE_ICON
	SubElement(template_data, "PreviewImage").text = PREVIEW_IMAGE
	template_content = SubElement(vstemplate, "TemplateContent")
	project_file = SubElement(template_content, "Project")
	project_file.set("TargetFileName", DEFAULT_NAME+".cppproj")
	project_file.set("File", DEFAULT_NAME+".cppproj")
	project_file.set("ReplaceParameters", "true")

	for root, dirs, files in os.walk(".", topdown=True):
		if(".\\." not in root):
			if(root == "."):
				base_content = project_file
			else:
				base_folder = root.split("\\")[-1]
				base_content = template_content.find(
					".//Folder[@Name='%s']" % base_folder
				)
			for directory in dirs:
				if directory[0] != ".":
					project_folder = SubElement(base_content, "Folder")
					project_folder.set("Name", directory)
					project_folder.set("TargetFolderName", directory)
			for filename in files:
				if not check_if_file_is_excluded(filename):
					if( filename[0:2] == "__" 
						or "vstemplate" in filename 
						or "cppproj" in filename):
						continue
					else:
						project_file = SubElement(base_content, "ProjectItem")
						project_file.text = filename
						project_file.set("TargetFileName", filename)
						if(".c" in filename 
							or ".h" in filename 
							or ".cpp" in filename):
							project_file.set("ReplaceParameters","true")
						else:
							project_file.set("ReplaceParameters","false")

	xml_string = ElementTree.tostring(vstemplate, 'utf-8')
	dom = xml.dom.minidom.parseString(xml_string) 
	pretty_xml_as_string = dom.toprettyxml()
	pretty_xml_as_string = pretty_xml_as_string[pretty_xml_as_string.find("?>")+2:]
	return pretty_xml_as_string


def generate_cpproj_content():
	raw_content = r"""<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003" ToolsVersion="14.0">
	 <PropertyGroup>
		 <SchemaVersion>2.0</SchemaVersion>
		 <ProjectVersion>7.0</ProjectVersion>
		 <ToolchainName>com.Atmel.AVRGCC8.CPP</ToolchainName>
		 <ProjectGuid>dce6c7e3-ee26-4d79-826b-08594b9ad897</ProjectGuid>
		 <avrdevice>ATmega32</avrdevice>
		 <avrdeviceseries>none</avrdeviceseries>
		 <OutputType>Executable</OutputType>
		 <Language>CPP</Language>
		 <OutputFileName>$(MSBuildProjectName)</OutputFileName>
		 <OutputFileExtension>.elf</OutputFileExtension>
		 <OutputDirectory>$(MSBuildProjectDirectory)\$(Configuration)</OutputDirectory>
		 <AssemblyName>"""+DEFAULT_NAME+r"""</AssemblyName>
		 <Name>"""+DEFAULT_NAME+r"""</Name>
		 <RootNamespace>"""+DEFAULT_NAME+r"""</RootNamespace>
		 <ToolchainFlavour>Native</ToolchainFlavour>
		 <KeepTimersRunning>true</KeepTimersRunning>
		 <OverrideVtor>false</OverrideVtor>
		 <CacheFlash>true</CacheFlash>
		 <ProgFlashFromRam>true</ProgFlashFromRam>
		 <RamSnippetAddress>0x20000000</RamSnippetAddress>
		 <UncachedRange />
		 <preserveEEPROM>true</preserveEEPROM>
		 <OverrideVtorValue>exception_table</OverrideVtorValue>
		 <BootSegment>2</BootSegment>
		 <ResetRule>0</ResetRule>
		 <eraseonlaunchrule>0</eraseonlaunchrule>
		 <EraseKey />
		 <avrtool>com.atmel.avrdbg.tool.ispmk2</avrtool>
		 <avrtoolserialnumber>000200049690</avrtoolserialnumber>
		 <avrdeviceexpectedsignature>0x1E9502</avrdeviceexpectedsignature>
		 <avrtoolinterface>ISP</avrtoolinterface>
		 <avrtoolinterfaceclock>2000000</avrtoolinterfaceclock>
		 <com_atmel_avrdbg_tool_ispmk2>
		   <ToolOptions>
			 <InterfaceProperties>
			   <IspClock>2000000</IspClock>
			 </InterfaceProperties>
			 <InterfaceName>ISP</InterfaceName>
		   </ToolOptions>
		   <ToolType>com.atmel.avrdbg.tool.ispmk2</ToolType>
		   <ToolNumber>000200049690</ToolNumber>
		   <ToolName>AVRISP mkII</ToolName>
		 </com_atmel_avrdbg_tool_ispmk2>
		 <AsfFrameworkConfig>
		   <framework-data xmlns="">
	  			<options />
			  <configurations />
			  <files />
			  <documentation help="" />
			  <offline-documentation help="" />
	  		  <dependencies>
		 			<content-extension eid="atmel.asf" uuidref="Atmel.ASF" version="3.40.0" />
	  		  </dependencies>
			</framework-data>
   	 </AsfFrameworkConfig>
 	</PropertyGroup>
  	<PropertyGroup Condition=" '$(Configuration)' == 'Release' ">
		<ToolchainSettings>
		   <AvrGccCpp>
			  <avrgcc.common.Device>-mmcu=atmega32 -B "%24(PackRepoDir)\atmel\ATmega_DFP\1.2.209\gcc\dev\atmega32"</avrgcc.common.Device>
			  <avrgcc.common.outputfiles.hex>True</avrgcc.common.outputfiles.hex>
			  <avrgcc.common.outputfiles.lss>True</avrgcc.common.outputfiles.lss>
			  <avrgcc.common.outputfiles.eep>True</avrgcc.common.outputfiles.eep>
			  <avrgcc.common.outputfiles.srec>True</avrgcc.common.outputfiles.srec>
			  <avrgcc.common.outputfiles.usersignatures>False</avrgcc.common.outputfiles.usersignatures>
			  <avrgcc.compiler.general.ChangeDefaultCharTypeUnsigned>True</avrgcc.compiler.general.ChangeDefaultCharTypeUnsigned>
			  <avrgcc.compiler.general.ChangeDefaultBitFieldUnsigned>True</avrgcc.compiler.general.ChangeDefaultBitFieldUnsigned>
			  <avrgcc.compiler.symbols.DefSymbols>
				 <ListValues>
					<Value>NDEBUG</Value>
					<Value>NDEBUG</Value>
				 </ListValues>
			  </avrgcc.compiler.symbols.DefSymbols>
			  <avrgcc.compiler.directories.IncludePaths>
				 <ListValues>
					<Value>%24(PackRepoDir)\atmel\ATmega_DFP\1.2.209\include</Value>
				 </ListValues>
			  </avrgcc.compiler.directories.IncludePaths>
			  <avrgcc.compiler.optimization.level>Optimize for size (-Os)</avrgcc.compiler.optimization.level>
			  <avrgcc.compiler.optimization.PackStructureMembers>True</avrgcc.compiler.optimization.PackStructureMembers>
			  <avrgcc.compiler.optimization.AllocateBytesNeededForEnum>True</avrgcc.compiler.optimization.AllocateBytesNeededForEnum>
			  <avrgcc.compiler.warnings.AllWarnings>True</avrgcc.compiler.warnings.AllWarnings>
			  <avrgcccpp.compiler.general.ChangeDefaultCharTypeUnsigned>True</avrgcccpp.compiler.general.ChangeDefaultCharTypeUnsigned>
			  <avrgcccpp.compiler.general.ChangeDefaultBitFieldUnsigned>True</avrgcccpp.compiler.general.ChangeDefaultBitFieldUnsigned>
			  <avrgcccpp.compiler.symbols.DefSymbols>
				 <ListValues>
					<Value>NDEBUG</Value>
					<Value>NDEBUG</Value>
				 </ListValues>
			  </avrgcccpp.compiler.symbols.DefSymbols>
			  <avrgcccpp.compiler.directories.IncludePaths>
				 <ListValues>
					<Value>%24(PackRepoDir)\atmel\ATmega_DFP\1.2.209\include</Value>
				 </ListValues>
			  </avrgcccpp.compiler.directories.IncludePaths>
			  <avrgcccpp.compiler.optimization.level>Optimize for size (-Os)</avrgcccpp.compiler.optimization.level>
			  <avrgcccpp.compiler.optimization.PackStructureMembers>True</avrgcccpp.compiler.optimization.PackStructureMembers>
			  <avrgcccpp.compiler.optimization.AllocateBytesNeededForEnum>True</avrgcccpp.compiler.optimization.AllocateBytesNeededForEnum>
			  <avrgcccpp.compiler.warnings.AllWarnings>True</avrgcccpp.compiler.warnings.AllWarnings>
			  <avrgcccpp.linker.libraries.Libraries>
				 <ListValues>
					<Value>libm</Value>
				 </ListValues>
			  </avrgcccpp.linker.libraries.Libraries>
			  <avrgcccpp.assembler.general.IncludePaths>
				 <ListValues>
					<Value>%24(PackRepoDir)\atmel\ATmega_DFP\1.2.209\include</Value>
				 </ListValues>
			  </avrgcccpp.assembler.general.IncludePaths>
			</AvrGccCpp>
  		</ToolchainSettings>
  	 </PropertyGroup>
  	 <PropertyGroup Condition=" '$(Configuration)' == 'Debug' ">
		<ToolchainSettings>
	  	<AvrGccCpp>
			  <avrgcc.common.Device>-mmcu=atmega32 -B "%24(PackRepoDir)\atmel\ATmega_DFP\1.2.209\gcc\dev\atmega32"</avrgcc.common.Device>
			  <avrgcc.common.outputfiles.hex>True</avrgcc.common.outputfiles.hex>
			  <avrgcc.common.outputfiles.lss>True</avrgcc.common.outputfiles.lss>
			  <avrgcc.common.outputfiles.eep>True</avrgcc.common.outputfiles.eep>
			  <avrgcc.common.outputfiles.srec>True</avrgcc.common.outputfiles.srec>
			  <avrgcc.common.outputfiles.usersignatures>False</avrgcc.common.outputfiles.usersignatures>
			  <avrgcc.compiler.general.ChangeDefaultCharTypeUnsigned>True</avrgcc.compiler.general.ChangeDefaultCharTypeUnsigned>
			  <avrgcc.compiler.general.ChangeDefaultBitFieldUnsigned>True</avrgcc.compiler.general.ChangeDefaultBitFieldUnsigned>
			  <avrgcc.compiler.symbols.DefSymbols>
				 <ListValues>
					<Value>DEBUG</Value>
					<Value>F_CPU=16000000</Value>
					<Value>DEBUG</Value>
				 </ListValues>
			  </avrgcc.compiler.symbols.DefSymbols>
			  <avrgcc.compiler.directories.IncludePaths>
				 <ListValues>
					<Value>%24(PackRepoDir)\atmel\ATmega_DFP\1.2.209\include</Value>
					<Value>../goldboard</Value>
				 </ListValues>
			  </avrgcc.compiler.directories.IncludePaths>
			  <avrgcc.compiler.optimization.level>Optimize (-O1)</avrgcc.compiler.optimization.level>
			  <avrgcc.compiler.optimization.PackStructureMembers>True</avrgcc.compiler.optimization.PackStructureMembers>
			  <avrgcc.compiler.optimization.AllocateBytesNeededForEnum>True</avrgcc.compiler.optimization.AllocateBytesNeededForEnum>
			  <avrgcc.compiler.optimization.DebugLevel>Default (-g2)</avrgcc.compiler.optimization.DebugLevel>
			  <avrgcc.compiler.warnings.AllWarnings>True</avrgcc.compiler.warnings.AllWarnings>
			  <avrgcccpp.compiler.general.ChangeDefaultCharTypeUnsigned>True</avrgcccpp.compiler.general.ChangeDefaultCharTypeUnsigned>
			  <avrgcccpp.compiler.general.ChangeDefaultBitFieldUnsigned>True</avrgcccpp.compiler.general.ChangeDefaultBitFieldUnsigned>
			  <avrgcccpp.compiler.symbols.DefSymbols>
				 <ListValues>
					<Value>DEBUG</Value>
					<Value>F_CPU=16000000</Value>
					<Value>DEBUG</Value>
				 </ListValues>
			  </avrgcccpp.compiler.symbols.DefSymbols>
			  <avrgcccpp.compiler.directories.IncludePaths>
				 <ListValues>
					<Value>%24(PackRepoDir)\atmel\ATmega_DFP\1.2.209\include</Value>
					<Value>../goldboard</Value>
				 </ListValues>
			  </avrgcccpp.compiler.directories.IncludePaths>
			  <avrgcccpp.compiler.optimization.level>Optimize (-O1)</avrgcccpp.compiler.optimization.level>
			  <avrgcccpp.compiler.optimization.PackStructureMembers>True</avrgcccpp.compiler.optimization.PackStructureMembers>
			  <avrgcccpp.compiler.optimization.AllocateBytesNeededForEnum>True</avrgcccpp.compiler.optimization.AllocateBytesNeededForEnum>
			  <avrgcccpp.compiler.optimization.DebugLevel>Default (-g2)</avrgcccpp.compiler.optimization.DebugLevel>
			  <avrgcccpp.compiler.warnings.AllWarnings>True</avrgcccpp.compiler.warnings.AllWarnings>
			  <avrgcccpp.compiler.miscellaneous.OtherFlags>-std=c++11</avrgcccpp.compiler.miscellaneous.OtherFlags>
			  <avrgcccpp.linker.libraries.Libraries>
					 <ListValues>
						<Value>libm</Value>
					 </ListValues>
  				</avrgcccpp.linker.libraries.Libraries>
				<avrgcccpp.assembler.general.IncludePaths>
 					<ListValues>
   					<Value>%24(PackRepoDir)\atmel\ATmega_DFP\1.2.209\include</Value>
 					</ListValues>
				</avrgcccpp.assembler.general.IncludePaths>
				<avrgcccpp.assembler.debugging.DebugLevel>Default (-Wa,-g)</avrgcccpp.assembler.debugging.DebugLevel>
			</AvrGccCpp>
		</ToolchainSettings>
	</PropertyGroup>
	<Import Project='$(AVRSTUDIO_EXE_PATH)\\Vs\\Compiler.targets' />
</Project>
"""
	parser = xml.etree.ElementTree.XMLParser(encoding="utf-8")
	cpproj = xml.etree.ElementTree.XML(raw_content, parser=parser)
	xml.etree.ElementTree.register_namespace("", "http://schemas.microsoft.com/developer/msbuild/2003")
	
	source_files = SubElement(cpproj, "ItemGroup")
	folders = SubElement(cpproj, "ItemGroup")
	other_files = SubElement(cpproj, "ItemGroup")
		
	for root, dirs, files in os.walk(".", topdown=True):
		root = root[2:]
		for directory in dirs:
			folder_name = root.replace("/","\\")+"\\"+directory
			if folder_name[0] == "\\":
				folder_name = folder_name[1:]
			if folder_name[0] != ".":
				SubElement(folders, "Folder").set("Include", folder_name)
		for filename in files:
			if not check_if_file_is_excluded(filename):
				if ".c" == filename[-2:] or ".cpp" == filename[-4:] or ".h" == filename[-2:]:
					source_file = SubElement(source_files, "Compile")
					filename = root.replace("/","\\")+"\\"+filename
					if filename[0] == "\\":
						filename = filename[1:]
					source_file.set("Include", filename)
					SubElement(source_file,"SubType").text = "compile"
				elif  ".exe" in filename or ".hex" in filename or ".conf" in filename:
					other_file = SubElement(other_files, "None")
					filename = root.replace("/","\\")+"\\"+filename
					if filename[0] == "\\":
						filename = filename[1:]
					other_file.set("Include", filename)
					SubElement(other_file,"SubType").text = "compile"
				
	xml_string = ElementTree.tostring(cpproj, 'utf-8')
	dom = xml.dom.minidom.parseString(xml_string) 
	pretty_xml_as_string = dom.toprettyxml()
	pretty_xml_as_string = pretty_xml_as_string.replace("&quot;","\"")
	return pretty_xml_as_string


def generate_AS7_template():
	mytemplate_content = generate_vs_template_content()
	with open("MyTemplate.vstemplate","w+") as  mytemplate_file:
		mytemplate_file.write(mytemplate_content)
			
	cppproj_content = generate_cpproj_content()
	cppproj_content = "".join([s for s in cppproj_content.splitlines(True) if s.strip()])
	with open(DEFAULT_NAME+".cppproj","w+") as  cppproj_file:
		cppproj_file.write(cppproj_content)

	zipf = zipfile.ZipFile("AS7-"+TEMPLATE_NAME+'.zip', 'w', zipfile.ZIP_DEFLATED)
	for root, dirs, files in os.walk("."):
		for file in files:
			if not check_if_file_is_excluded(file) and ".\." not in root:
				zipf.write(os.path.join(root, file))
	zipf.close()
	os.remove("MyTemplate.vstemplate")
	os.remove(DEFAULT_NAME+".cppproj")

def generate_VSC_template():
	ini_file_content = """[env:main]
# Uebertragen mit Bootloader
#upload_protocol = arduino
#upload_port = COM3

# Uebertragen mit AVR ISP MKII
upload_protocol = avrispmkii
upload_port = usb

# Uebertragen mit AVR ISP STK500
#upload_protocol = stk500
#upload_port = COM3

platform = atmelavr
board = ATmega32
lib_compat_mode = off
build_flags = -I/lib/goldboard
board_build.f_cpu = 16000000UL
board_bootloader.file = bootloader_mega32_optiboot.hex
board_bootloader.low_fuses = 0xFF
board_bootloader.high_fuses = 0xDE"""

	with open("platformio.ini","w+") as ini_file:
		ini_file.write(ini_file_content)
	zipf = zipfile.ZipFile("VSC-"+TEMPLATE_NAME+'.zip', 'w', zipfile.ZIP_DEFLATED)
	for _, _, files in os.walk("goldboard"):
		for file in files:
			zipf.write(os.path.join("goldboard", file), os.path.join("lib/goldboard", file))
	zipf.write("main.cpp", os.path.join("src", "main.cpp"))
	zipf.write("platformio.ini")
	zipf.write("utils/bootloader_mega32_optiboot.hex","bootloader_mega32_optiboot.hex")
	zipf.close()
	os.remove("platformio.ini")


if __name__ == '__main__':
	generate_AS7_template()
	generate_VSC_template()