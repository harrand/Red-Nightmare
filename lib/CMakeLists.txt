project(soloud)
add_library(soloud
	# always need core
	soloud/src/core/soloud_audiosource.cpp
	soloud/src/core/soloud_bus.cpp
	soloud/src/core/soloud_core_3d.cpp
	soloud/src/core/soloud_core_basicops.cpp
	soloud/src/core/soloud_core_faderops.cpp
	soloud/src/core/soloud_core_getters.cpp
	soloud/src/core/soloud_core_setters.cpp
	soloud/src/core/soloud_core_voicegroup.cpp
	soloud/src/core/soloud_core_voiceops.cpp
	soloud/src/core/soloud_fader.cpp
	soloud/src/core/soloud_fft_lut.cpp
	soloud/src/core/soloud_fft.cpp
	soloud/src/core/soloud_file.cpp
	soloud/src/core/soloud_filter.cpp
	soloud/src/core/soloud_misc.cpp
	soloud/src/core/soloud_queue.cpp
	soloud/src/core/soloud_thread.cpp
	soloud/src/core/soloud.cpp
	# need one backend, and one audiosource
	# in this case, wasapi for windows
	soloud/src/backend/wasapi/soloud_wasapi.cpp
	# and audiosource/wav
	soloud/src/audiosource/wav/dr_flac.h
	soloud/src/audiosource/wav/dr_impl.cpp
	soloud/src/audiosource/wav/dr_mp3.h
	soloud/src/audiosource/wav/dr_wav.h
	soloud/src/audiosource/wav/soloud_wav.cpp
	soloud/src/audiosource/wav/soloud_wavstream.cpp
	soloud/src/audiosource/wav/stb_vorbis.c
	soloud/src/audiosource/wav/stb_vorbis.h
	)

target_include_directories(soloud PUBLIC soloud/include)
target_compile_definitions(soloud PRIVATE -DWITH_WASAPI)