project "App"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")
   staticruntime "off"

   files { 
      "Source/**.h", 
      "Source/**.cpp", 
      "../Vendor/imgui/**.cpp", 
      "../Vendor/imgui/**.h", 
   }

   includedirs
   {
      "Source",
      "../Vendor/glm",
      "../Vendor/GLEW/include",
      "../Vendor/GLFW/include",
      "../Vendor/imgui"
   }

   libdirs
   {
      "../Vendor/GLEW",
      "../Vendor/GLFW"
   }

   links
   {
      "glew32",   
      "glfw3",    
      "opengl32"
   }

   filter "system:windows"
      systemversion "latest"
      defines { "WINDOWS", "SHADERS_DIR=\"App/Source/shaders\""}
      
      postbuildcommands {
         "{COPY} \"../Vendor/GLEW/glew32.dll\" \"%{cfg.targetdir}\""
      }

   filter "configurations:Debug"
      defines { "DEBUG" }
      runtime "Debug"
      symbols "On"

   filter "configurations:Release"
      defines { "RELEASE" }
      runtime "Release"
      optimize "On"
      symbols "On"

   filter "configurations:Dist"
      defines { "DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"
