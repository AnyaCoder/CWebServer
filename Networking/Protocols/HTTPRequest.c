
#include "HTTPRequest.h"

#include "../../DataStructures/Lists/Queue.h"
#include "../../thirdparty/cJSON/cJSON.h"

#include <string.h>
#include <stdlib.h>


void extract_request_line_fields(struct HTTPRequest* request, char* request_line);
void extract_header_fields(struct HTTPRequest* request, char* header_fields);
void extract_body(struct HTTPRequest* request, char* body);

struct HTTPRequest http_request_constructor(
	char* request_string
)
{

	struct HTTPRequest request;
	char* requested = (char*)malloc(sizeof(char) * (strlen(request_string) + 1));
	strcpy(requested, request_string);
	for (int i = 0; i < strlen(requested) - 4; i++)
	{

		if (requested[i] == '\r' && requested[i + 1] == '\n'
			&& requested[i + 2] == '\r' && requested[i + 3] == '\n')
		{
			requested[i + 2] = ' ';
			requested[i + 3] = '|';
		}
	}

	char* request_line = strtok(requested, "\n");
    char* header_fields = strtok(NULL, "|");
	char* body = strtok(NULL, "\0");
	
	extract_request_line_fields(&request, request_line);
	extract_header_fields(&request, header_fields);
	extract_body(&request, body);
	
	return request;

}

void http_request_destructor(
	struct HTTPRequest* request
)
{
	dictionary_destructor(&request->request_line);
	dictionary_destructor(&request->header_fields);
	dictionary_destructor(&request->body);
}


void extract_request_line_fields(struct HTTPRequest* request, char* request_line)
{
	char* fields = (char*)malloc(sizeof(char) * (strlen(request_line) + 1));
	strcpy(fields, request_line);

	char* method = strtok(fields, " ");
	char* uri = strtok(NULL, " ");
	char* http_version = strtok(NULL, "\r");

	struct Dictionary request_line_dict = dictionary_constructor(compare_string_keys);
	// must be sizeof(char) * (strlen(¡¤) + 1)
	request_line_dict.insert(&request_line_dict, "method", sizeof("method"), method, sizeof(char) * (strlen(method) + 1));
	request_line_dict.insert(&request_line_dict, "uri", sizeof("uri"), uri, sizeof(char) * (strlen(uri) + 1));
	request_line_dict.insert(&request_line_dict, "http_version", sizeof("http_version"), http_version, sizeof(char) * (strlen(http_version) + 1));
	request->request_line = request_line_dict;
}

void extract_header_fields(struct HTTPRequest* request, char* header_fields)
{
	char* fields = (char*)malloc(sizeof(char) * (strlen(header_fields) + 1));
	strcpy(fields, header_fields);

	struct Queue headers = queue_constructor();
	char* field = strtok(fields, "\n");
	while (field)
	{
		headers.push(&headers, field, sizeof(char) * (strlen(field) + 1));
		field = strtok(NULL, "\n");
	}
	request->header_fields = dictionary_constructor(compare_string_keys);
	char* header = (char*)headers.front(&headers);
	while (header)
	{
		char* key = strtok(header, ":");
		char* value = strtok(NULL, "\r");
		if (value)
		{
			if (value[0] == ' ')
			{
				value++;
			}
			request->header_fields.insert(
				&request->header_fields, 
				key, sizeof(char) * (strlen(key) + 1),
				value, sizeof(char) * (strlen(value) + 1)
			);

		}
		headers.pop(&headers);
		if (headers.size(&headers) > 0)
		{
			header = (char*)headers.front(&headers);
		}
		else {
			header = NULL;
		}
	}
	queue_destructor(&headers);
}

void extract_body(struct HTTPRequest* request, char* body)
{
	char* content_type = (char*)request->header_fields.search(&request->header_fields, "Content-Type", sizeof("Content-Type"));
	if (content_type)
	{
		struct Dictionary body_fields = dictionary_constructor(compare_string_keys);
		if (strcmp(content_type, "application/x-www-form-urlencoded") == 0)
		{
			// Collect each key value pair as a set and store them in a queue.
			struct Queue fields = queue_constructor();
			char* field = strtok(body, "&");
			while (field)
			{
				fields.push(&fields, field, sizeof(char) * (strlen(field) + 1));
			}
			// Iterate over the queue to further separate keys from values.
			field = fields.front(&fields);
			while (field)
			{
				char* key = strtok(field, "=");
				char* value = strtok(NULL, "\0");
				// Remove unnecessary leading white space.
				if (value[0] == ' ')
				{
					value++;
				}
				// Insert the key value pair into the dictionary.
				body_fields.insert(&body_fields, key, sizeof(char) * (strlen(key) + 1), value, sizeof(char) * (strlen(value) + 1));
				// Collect the next item in the queue.
				fields.pop(&fields);
				field = fields.front(&fields);
			}
			// Destroy the queue.
			queue_destructor(&fields);
		}
		else if (strcmp(content_type, "application/json") == 0)
		{
			// Parse JSON body using cJSON library
			cJSON* json = cJSON_Parse(body);
			if (json == NULL)
			{
				fprintf(stderr, "Error parsing JSON\n");
			}
			else
			{
				// Iterate over all keys in the JSON object
				cJSON* item = json->child;
				while (item)
				{
					char* key = item->string;
					char* value = NULL;

					if (cJSON_IsString(item) && item->valuestring)
					{
						value = item->valuestring;
					}
					else
					{
						// Convert non-string JSON values (like numbers, booleans, arrays) to strings
						value = cJSON_Print(item);
					}

					if (key && value)
					{
						body_fields.insert(
							&body_fields,
							key, sizeof(char) * (strlen(key) + 1),
							value, sizeof(char) * (strlen(value) + 1)
						);

						// If value is dynamically allocated (like cJSON_Print), free it after use
						if (!cJSON_IsString(item))
						{
							free(value);
						}
					}
					item = item->next;
				}
				cJSON_Delete(json);  // Clean up the parsed JSON object
			}
		}
		else
		{
			// Save the data as a single key value pair.
			body_fields.insert(&body_fields, "data", sizeof("data"), body, sizeof(char) * (strlen(body) + 1));
		}
		// Set the request's body dictionary.
		request->body = body_fields;
	}
}

