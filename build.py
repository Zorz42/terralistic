import os
import sys
import shutil

project_path = os.path.dirname(os.path.realpath(__file__)) + "/"


def createDir(path):
    os.makedirs(project_path + path, exist_ok=True)


if sys.platform == "darwin":
    createDir(project_path + "Dependencies/")
    if not os.path.exists(project_path + "Dependencies/MacOS/"):
        os.system(f"git clone https://github.com/Zorz42/Terralistic-MacOS-Dependencies {project_path}Dependencies/MacOS/")
    else:
        os.system(f"git -C {project_path}Dependencies/MacOS/ pull --rebase")

    os.system(f"xcodebuild build -project {project_path}Terralistic.xcodeproj -scheme Terralistic BUILD_DIR={project_path}Temp")
    os.system(f"xcodebuild build -project {project_path}Terralistic.xcodeproj -scheme Terralistic-server BUILD_DIR={project_path}Temp")

    createDir("Output/MacOS/")

    shutil.rmtree(project_path + "Output/MacOS/Terralistic.app/", ignore_errors=True)
    shutil.move(project_path + "Temp/Release/Terralistic.app/", project_path + "Output/MacOS/")
    if os.path.exists(project_path + "Output/MacOS/Terralistic-server"):
        os.remove(project_path + "Output/MacOS/Terralistic-server")
    shutil.move(project_path + "Temp/Release/Terralistic-server", project_path + "Output/MacOS/")
    shutil.copy(project_path + "Client/Resources/Structures.asset", project_path + "Output/MacOS/")
    shutil.rmtree(project_path + "Temp/")
elif sys.platform == "linux":
    os.system("sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev")

    createDir("Build/")
    os.system(f"cd {project_path}Build/ && cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++ .. && make -j$(nproc)")

    createDir("Output/Linux/Terralistic")

    if os.path.exists(project_path + "Output/Linux/Terralistic/Terralistic"):
        os.remove(project_path + "Output/Linux/Terralistic/Terralistic")
    shutil.move(project_path + "Build/Terralistic", project_path + "Output/Linux/Terralistic/")
    shutil.rmtree(project_path + "Output/Linux/Terralistic/Resources/", ignore_errors=True)
    shutil.move(project_path + "Build/Resources/", project_path + "Output/Linux/Terralistic/")

    shutil.copy("/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0", project_path + "Output/Linux/Terralistic/")
    shutil.copy("/usr/lib/x86_64-linux-gnu/libSDL2_image-2.0.so.0", project_path + "Output/Linux/Terralistic/")
    shutil.copy("/usr/lib/x86_64-linux-gnu/libSDL2_image-2.0.so.0", project_path + "Output/Linux/Terralistic/")

    shutil.copy(project_path + "Build/Terralistic-server", project_path + "Output/Linux/Terralistic/")
    shutil.copy(project_path + "Build/Structures.asset", project_path + "Output/Linux/Terralistic/")
elif sys.platform == "win32":
    createDir("Dependencies/")
    if not os.path.exists(project_path + "Dependencies/Windows/"):
        os.system(f"git clone https://github.com/Zorz42/Terralistic-Windows-Dependencies {project_path}Dependencies/Windows/")
    else:
        os.system(f"git -C {project_path}Dependencies/Windows/ pull --rebase")

    createDir("Build/")
    cmake_path = "C:\\msys64\\mingw64\\bin\\cmake.exe"

    os.system(f"cd {project_path}Build/ && {cmake_path} .. && {cmake_path} --build .")

    if os.path.exists(project_path + "Output/Windows/Terralistic/"):
        shutil.rmtree(project_path + "Output/Windows/Terralistic/")

    createDir("Output/Windows/Terralistic/")
    shutil.move(project_path + "Build/Debug/Terralistic.exe", project_path + "Output/Windows/Terralistic/")
    for file in os.listdir(project_path + "Build/"):
        if file.endswith(".dll"):
            shutil.move(project_path + "Build/" + file, project_path + "Output/Windows/Terralistic/")

    shutil.rmtree(project_path + "Output/Windows/Terralistic/Resources/", ignore_errors=True)
    shutil.move(project_path + "Build/Resources/", project_path + "Output/Windows/Terralistic/Resources/")

    if sys.argv[1] == "run":
        os.system(project_path + "Output/Windows/Terralistic/Terralistic.exe")
else:
    print("Your current platform is not yet supported by this build script!")
