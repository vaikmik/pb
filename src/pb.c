#include <evhttp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snippet.h"
#include "types.h"

static void on_signal(evutil_socket_t fd, short event, void *arg);

static void on_request(struct evhttp_request *request, void *arg);
static void on_get(struct evhttp_request *request);
static void on_post(struct evhttp_request *request);

int
main(int argc, char *argv[])
{
    int port;
    struct event_base *evbase;
    struct evhttp *server;
    struct event *sigint;

    if (argc < 2)
    {
        fprintf(stderr, "USAGE: pb [PORT]\n");
        return -1;
    }

    if (db_init("db.idx", "db.rec") != OK)
    {
        fprintf(stderr, "db_init()\n");
        return -1;
    }

    port = strtol(argv[1], NULL, 10);
    evbase = event_base_new();
    server = evhttp_new(evbase);
    
    evhttp_set_allowed_methods(server, EVHTTP_REQ_GET|EVHTTP_REQ_POST);
    evhttp_set_max_body_size(server, 8096);
    evhttp_set_max_headers_size(server, 2048);
    evhttp_set_timeout(server, 5);
    evhttp_set_gencb(server, on_request, NULL);
    if (evhttp_bind_socket(server, "127.0.0.1", port) != 0)
    {
        fprintf(stderr, "evhttp_bind_socket()\n");
        return -1;
    }

    /* Signal handling */
    sigint = evsignal_new(evbase, SIGINT, on_signal, evbase);
    event_add(sigint, NULL);

    event_base_dispatch(evbase);

    event_del(sigint);

    fprintf(stderr, "exiting...\n");

    evhttp_free(server);
    event_base_free(evbase);
    db_destroy();

    return 0;
}

static void
on_request(struct evhttp_request *request, void *arg)
{
    if (!request) 
    {
        return;
    }

    if(request->type == EVHTTP_REQ_GET)
    {
        on_get(request);
    }
    else if(request->type == EVHTTP_REQ_POST)
    {
        on_post(request);
    }
}

static void
on_get(struct evhttp_request *request)
{
    struct dbrec snippet;
    struct evbuffer *buffer;
    const char *uri;
    usize id;

    uri = evhttp_request_get_uri(request);
    if(strlen(uri) == 1)
    {
        /* '/', Should display some kind of a usage page */
        evhttp_send_reply(request, HTTP_BADREQUEST, "Bad Request", NULL);
        return;
    }

    id = strtoull(uri + 1, NULL, 10);
    if (snippet_get(&snippet, id) != OK)
    {
        evhttp_send_reply(request, HTTP_NOTFOUND, "Not Found", NULL);
        return;
    }

    buffer = evbuffer_new();
    evbuffer_add(buffer, snippet.data, snippet.len);
    evhttp_send_reply(request, HTTP_OK, "OK", buffer);
    evbuffer_free(buffer);
    dbrec_free(&snippet);
}

static void
on_post(struct evhttp_request *request)
{
    struct dbidx snippet;
    struct evbuffer *buffer;
    char *data;
    usize len;

    buffer = evhttp_request_get_input_buffer(request);
    len = evbuffer_get_length(buffer);

    data = calloc(1, len);
    evbuffer_remove(buffer, data, len);

    if (snippet_init(&snippet, (const u8 *)data, len) != OK)
    {
        evhttp_send_reply(request, HTTP_INTERNAL, "Internal Server Error", NULL);
        goto out;
    }

    evbuffer_add_printf(buffer, "id: %lu", snippet.id);
    evhttp_send_reply(request, HTTP_OK, "OK", buffer);

out:
    free(data);
}

static void
on_signal(evutil_socket_t fd, short event, void *arg)
{
    event_base_loopbreak(arg);
}
