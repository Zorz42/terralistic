import os
import sys
import shutil
import urllib.request
import tarfile
import zipfile

project_path = os.path.dirname(os.path.realpath(__file__)) + "/"


def createDir(path):
    os.makedirs(project_path + path, exist_ok=True)


if sys.platform == "darwin":
    createDir(project_path + "Dependencies/")

    if not os.path.exists(project_path + "Dependencies/SFML-2.5.1-macos-clang/"):
        print("Downloading SFML libraries")

        sfml_url = "https://www.sfml-dev.org/files/SFML-2.5.1-macOS-clang.tar.gz"
        sfml_file = project_path + "sfml.tar.gz"

        with urllib.request.urlopen(sfml_url) as sfml_request:
            with open(sfml_file, 'wb') as sfml_download:
                sfml_download.write(sfml_request.read())

        with tarfile.open(sfml_file, "r:gz") as sfml_tar:
            sfml_tar.extractall(project_path + "Dependencies/")

        os.remove(sfml_file)

    os.system(f"xcodebuild build -quiet -project {project_path}Terralistic.xcodeproj -scheme Terralistic BUILD_DIR={project_path}Temp")
    os.system(f"xcodebuild build -quiet -project {project_path}Terralistic.xcodeproj -scheme Terralistic-server BUILD_DIR={project_path}Temp")

    createDir("Output/MacOS/")

    shutil.rmtree(project_path + "Output/MacOS/Terralistic.app/", ignore_errors=True)
    shutil.move(project_path + "Temp/Release/Terralistic.app/", project_path + "Output/MacOS/")

    shutil.rmtree(project_path + "Output/MacOS/Terralistic-server.app/", ignore_errors=True)
    shutil.move(project_path + "Temp/Release/Terralistic-server.app/", project_path + "Output/MacOS/")

    shutil.rmtree(project_path + "Temp/")


elif sys.platform == "linux":
    createDir("Dependencies/")

    if not os.path.exists(project_path + "Dependencies/SFML-2.5.1/"):
        print("Downloading SFML libraries")

        sfml_url = "https://www.sfml-dev.org/files/SFML-2.5.1-linux-gcc-64-bit.tar.gz"
        sfml_file = project_path + "sfml.tar.gz"

        with urllib.request.urlopen(sfml_url) as sfml_request:
            with open(sfml_file, 'wb') as sfml_download:
                sfml_download.write(sfml_request.read())

        with tarfile.open(sfml_file, "r") as sfml_tar:
            sfml_tar.extractall(project_path + "Dependencies/")

        os.remove(sfml_file)

    createDir("Build/")
    os.system(f"cd {project_path}Build/ && cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++ .. && make -j$(nproc)")

    createDir("Output/Linux/Terralistic")

    if os.path.exists(project_path + "Output/Linux/Terralistic/Terralistic"):
        os.remove(project_path + "Output/Linux/Terralistic/Terralistic")
    shutil.move(project_path + "Build/Terralistic", project_path + "Output/Linux/Terralistic/")
    shutil.rmtree(project_path + "Output/Linux/Terralistic/Resources/", ignore_errors=True)
    shutil.move(project_path + "Build/Resources/", project_path + "Output/Linux/Terralistic/")

    #for lib_file in lib_files:
        #shutil.copy(lib_file, project_path + "Output/Linux/Terralistic/")

    shutil.copy(project_path + "Build/Terralistic-server", project_path + "Output/Linux/Terralistic/")
    shutil.copy(project_path + "Build/Structures.asset", project_path + "Output/Linux/Terralistic/")


elif sys.platform == "win32":
    createDir("Dependencies/")

    if not os.path.exists(project_path + "Dependencies/SFML-2.5.1/"):
        print("Downloading SFML libraries")

        sfml_url = "https://www.sfml-dev.org/files/SFML-2.5.1-windows-vc15-64-bit.zip"
        sfml_file = project_path + "sfml.zip"

        with urllib.request.urlopen(sfml_url) as sfml_request:
            with open(sfml_file, 'wb') as sfml_download:
                sfml_download.write(sfml_request.read())

        with zipfile.ZipFile(sfml_file, "r") as sfml_zip:
            sfml_zip.extractall(project_path + "Dependencies/")

        os.remove(sfml_file)

    createDir("Build/")
    cmake_path = "\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\CMake\\CMake\\bin\\cmake.exe\""

    os.system(f"cd {project_path}Build/ && {cmake_path} -G \"Visual Studio 16 2019\" -T ClangCL -A x64 -DCMAKE_BUILD_TYPE=Release .. && {cmake_path} --build .")

    if os.path.exists(project_path + "Output/Windows/Terralistic/"):
        shutil.rmtree(project_path + "Output/Windows/Terralistic/")

    createDir("Output/Windows/Terralistic/")
    shutil.move(project_path + "Build/Debug/Terralistic.exe", project_path + "Output/Windows/Terralistic/")
    shutil.move(project_path + "Build/Debug/Terralistic-server.exe", project_path + "Output/Windows/Terralistic/")

    for file in os.listdir(project_path + "Build/"):
        if file.endswith(".dll"):
            shutil.move(project_path + "Build/" + file, project_path + "Output/Windows/Terralistic/")

    shutil.rmtree(project_path + "Output/Windows/Terralistic/Resources/", ignore_errors=True)
    shutil.move(project_path + "Build/Resources/", project_path + "Output/Windows/Terralistic/Resources/")

    if len(sys.argv) != 1 and sys.argv[1] == "run":
        os.system(project_path + "Output/Windows/Terralistic/Terralistic.exe")
else:
    print("Your current platform is not yet supported by this build script!")
