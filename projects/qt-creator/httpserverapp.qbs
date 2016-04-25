import qbs

Project {
	DynamicLibrary {
		name: "httpserverapp"

		Depends { name: "cpp" }
		cpp.cxxLanguageVersion: "c++11"

		cpp.defines: qbs.buildVariant == "debug" ? base : base.concat(["DEBUG"])

		cpp.dynamicLibraries: base.concat(["gnutls"])

		Properties {
			condition: qbs.targetOS.contains("linux")
			cpp.defines: outer.concat(["POSIX"])
			cpp.dynamicLibraries: outer.concat(["dl", "pthread"])
		}

		Properties {
			condition: qbs.targetOS.contains("windows")
			cpp.defines: outer.concat(["WIN32"])
		}

		files: [
			"../../src/FileIncoming.cpp",
			"../../src/FileIncoming.h",
			"../../src/Main.cpp",
			"../../src/Main.h",
			"../../src/RawData.h",
			"../../src/ServerRequest.h",
			"../../src/ServerResponse.h",
			"../../src/Socket.cpp",
			"../../src/Socket.h",
			"../../src/SocketAdapter.cpp",
			"../../src/SocketAdapter.h",
			"../../src/SocketAdapterDefault.cpp",
			"../../src/SocketAdapterDefault.h",
			"../../src/SocketAdapterTls.cpp",
			"../../src/SocketAdapterTls.h",
			"../../src/System.cpp",
			"../../src/System.h",
			"../../src/Test.cpp",
			"../../src/Test.h",
			"../../src/Utils.cpp",
			"../../src/Utils.h",
		]
	}
}
