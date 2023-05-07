(declare-project :name "midi")

(def os (os/which))

(def os-compiler-flags
  (case os
    :macos ["-D__MACOSX_CORE__"]
    :linux ["-D__UNIX_JACK__"]
    (errorf "need to define compiler flags for %q" os)))

(def os-linker-flags
  (case os
    :macos ["-framework" "CoreMIDI" "-framework" "CoreAudio" "-framework" "CoreFoundation"]
    :linux ["-ljack"]
    (errorf "need to define linker flags for %q" os)))

(declare-native
  :name "midi"
  :source ["src/main.cpp" "rtmidi/RtMidi.cpp"]
  :cppflags ["-Irtmidi" "-std=c++14" ;os-compiler-flags]
  :lflags os-linker-flags)
