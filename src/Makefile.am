lib_LIBRARIES = librpc.a
librpc_a_SOURCES = client.c message.c message_parse_raw.c message_parse_http.c message_reply_http.c send.c send_raw.c server.c string.c
include_HEADERS = rpc.h

bin_PROGRAMS = rpcd rpcc
rpcd_SOURCES = rpcd.c
rpcd_LDADD = librpc.a

rpcc_SOURCES = rpcc.c
rpcc_LDADD = librpc.a

bin_PROGRAMS += fuzz_message_parse_http
fuzz_message_parse_http_SOURCES = fuzz_message_parse_http.cc
fuzz_message_parse_http_CXXFLAGS = -fsanitize=address,fuzzer
fuzz_message_parse_http_LDADD = librpc.a
