from platformio import util
import os
Import("env")

config = util.load_project_config()

bootloader_file = config.get("env:main", "bootloader_file")
fuse_high_byte = config.get("env:main", "fuse_high_byte")
fuse_low_byte = config.get("env:main", "fuse_low_byte")
programmer = config.get("env:main", "programmer")
goldboard_repository = config.get("env:main", "goldboard_repository")
simulator_file = config.get("env:main", "simulator")

avrdude_command = "$UPLOADER -v -C " + os.path.join(env["PIOHOME_DIR"],"packages","tool-avrdude","avrdude.conf") + " "

if programmer == "avrispmkii":
    parameter = "-p m32 -c avrispmkii -P usb"
elif programmer == "stk500":
    port = config.get("env:main", "port")
    parameter = "-p m32 -c stk500 -P "+port
elif programmer == "arduino":
    port = config.get("env:main", "port")
    parameter = "-p m32 -c arduino -b 115200 -P "+port
else:
    print programmer+" ist kein gueltiger programmer! waehle [stk500, avrispmkii oder arduino]"
    exit(1)

def on_upload_bootloader(source, target, env):
    exit(env.Execute(avrdude_command + parameter
                    + "  -U flash:w:" + bootloader_file +":i"))

def on_upload_fuse(source, target, env):
    exit(env.Execute(avrdude_command + parameter
                     + " -U lfuse:w:"+fuse_low_byte+":m -U hfuse:w:"+fuse_high_byte+":m"))

def on_upload_flash(source, target, env):
    firmware_path = str(source[0])
    exit(env.Execute(avrdude_command + parameter
                     + "  -U flash:w:" + firmware_path+":i"))

def on_upload_eeprom(source, target, env):
    print "EEPROM ist noch nicht implementiert. Frag U was du machen sollst."
    exit(1)

def on_update(source, target, env):
    try:
        import urllib2
        import zipfile
        import shutil
        src = "goldboard4-master/goldboard"
        dest = "lib/goldboard"
        print "connecting"
        response = urllib2.urlopen(goldboard_repository)
        print "download"
        html = response.read()
        with open("tmp.zip", "wb") as f:
            f.write(html)
        print "extract"
        with zipfile.ZipFile("tmp.zip","r") as zip:
            zip.extractall()
        print "delete existing"
        if os.path.isdir(dest):
            shutil.rmtree(dest)
        os.mkdir(dest)
        print "copy new"
        src_files = os.listdir(src)
        for file_name in src_files:
            full_file_name = os.path.join(src, file_name)
            full_dest = os.path.join(dest, file_name)
            if (os.path.isfile(full_file_name)):
                shutil.copy(full_file_name, full_dest)
        print "cleanup"
        shutil.rmtree("goldboard4-master")
        os.remove("tmp.zip")
    except:
        exit(1)


def on_simulate(source, target, env):
    exit(env.Execute(simulator_file))

env.Replace(UPLOADHEXCMD=on_upload_flash)
env.Replace(PROGRAMHEXCMD=on_upload_flash)
env.Replace(UPLOADEEPCMD=on_upload_eeprom)
env.AlwaysBuild(env.Alias("fuse", None, on_upload_fuse))
env.AlwaysBuild(env.Alias("uploadboot", None, on_upload_bootloader))
env.AlwaysBuild(env.Alias("update", None, on_update))
env.AlwaysBuild(env.Alias("simulate", None, on_simulate))
