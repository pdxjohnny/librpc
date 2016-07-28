noinst_LIBRARIES = librpc.a
librpc_a_SOURCES = client.c message.c message_parse_http.c message_reply_http.c server.c string.c
include_HEADERS = rpc.h

bin_PROGRAMS = rpcd
rpcd_SOURCES = rpcd.c
rpcd_LDADD = librpc.a
