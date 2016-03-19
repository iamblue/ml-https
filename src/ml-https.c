#include <string.h>
#include "jerry.h"
#include "httpclient.h"
#include "microlattice.h"

DELCARE_HANDLER(https) {
  if (args_cnt == 2 && args_p[0].type == JERRY_API_DATA_TYPE_OBJECT) {
    jerry_api_value_t method;
    jerry_api_value_t header;
    jerry_api_value_t url;

    // method
    jerry_api_get_object_field_value (args_p[0].v_object, (jerry_api_char_t *) "method", &method);
    // url
    jerry_api_get_object_field_value (args_p[0].v_object, (jerry_api_char_t *) "url", &url);

    /* method */
    int method_req_sz = jerry_api_string_to_char_buffer(method.v_string, NULL, 0);
    method_req_sz *= -1;
    char method_buffer [method_req_sz+1];
    method_req_sz = jerry_api_string_to_char_buffer (method.v_string, (jerry_api_char_t *) method_buffer, method_req_sz);
    method_buffer[method_req_sz] = '\0';

    /* url */
    int url_req_sz = jerry_api_string_to_char_buffer(url.v_string, NULL, 0);
    url_req_sz *= -1;
    char url_buffer [url_req_sz+1];
    url_req_sz = jerry_api_string_to_char_buffer (url.v_string, (jerry_api_char_t *) url_buffer, url_req_sz);
    url_buffer[url_req_sz] = '\0';

    httpclient_t client = {0};
    httpclient_data_t client_data = {0};

    // char *buf;
    // buf = pvPortMalloc(BUF_SIZE);
    // if (buf == NULL) {
    //     printf("httpclient_test malloc failed.\r\n");
    //     return true;
    // }

    char buf[100] = {0};
    client_data.response_buf = buf;
    client_data.response_buf_len = 100;

    // header
    jerry_api_get_object_field_value (args_p[0].v_object, (jerry_api_char_t *) "header", &header);

    /* header */
    int header_req_sz = jerry_api_string_to_char_buffer(header.v_string, NULL, 0);
    header_req_sz *= -1;
    char header_buffer [header_req_sz+1];
    header_req_sz = jerry_api_string_to_char_buffer (header.v_string, (jerry_api_char_t *) header_buffer, header_req_sz);
    header_buffer[header_req_sz] = '\0';

    httpclient_set_custom_header(&client, header_buffer);

    if (strncmp (method_buffer, "POST", (size_t)method_req_sz) == 0) {
      jerry_api_value_t data;
      jerry_api_value_t contentType;

      // data
      jerry_api_get_object_field_value (args_p[0].v_object, (jerry_api_char_t *) "data", &data);
      // contentType
      jerry_api_get_object_field_value (args_p[0].v_object, (jerry_api_char_t *) "contentType", &contentType);

      /* data */
      int data_req_sz = jerry_api_string_to_char_buffer(data.v_string, NULL, 0);
      data_req_sz *= -1;
      char data_buffer [data_req_sz+1];
      data_req_sz = jerry_api_string_to_char_buffer (data.v_string, (jerry_api_char_t *) data_buffer, data_req_sz);
      data_buffer[data_req_sz] = '\0';

      /* contentType */
      int contentType_req_sz = jerry_api_string_to_char_buffer(contentType.v_string, NULL, 0);
      contentType_req_sz *= -1;
      char contentType_buffer [contentType_req_sz+1];
      contentType_req_sz = jerry_api_string_to_char_buffer (contentType.v_string, (jerry_api_char_t *) contentType_buffer, contentType_req_sz);
      contentType_buffer[contentType_req_sz] = '\0';

      client_data.post_content_type = contentType_buffer;
      client_data.post_buf = data_buffer;
      client_data.post_buf_len = strlen(data_buffer);

      httpclient_post(&client, url_buffer, HTTPS_PORT, &client_data);
      jerry_api_release_object(&data);
      jerry_api_release_object(&contentType);

    } else if (strncmp (method_buffer, "GET", (size_t)method_req_sz) == 0) {
      httpclient_get(&client, url_buffer, HTTPS_PORT, &client_data);
    }

    jerry_api_value_t params[0];
    params[0].type = JERRY_API_DATA_TYPE_STRING;
    params[0].v_string = jerry_api_create_string (client_data.response_buf);
    jerry_api_call_function(args_p[1].v_object, NULL, false, &params, 1);
    jerry_api_release_value(&params);

    httpclient_close(&client, HTTPS_PORT);
    ret_val_p->type = JERRY_API_DATA_TYPE_BOOLEAN;
    ret_val_p->v_bool = true;

    // vPortFree(buf);
    jerry_api_release_object(&header);
    jerry_api_release_object(&method);
    jerry_api_release_object(&url);
    return true;
  }
}


void ml_http_init(void) {
  REGISTER_HANDLER(https);
}