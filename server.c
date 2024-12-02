#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1048567600

const char *get_file_extension(const char *file_name) {
  const char *dot = strrchr(file_name, ".");
  if (!dot || dot == file_name) {
    return "";
  }
  return dot + 1;
} 

const char *get_mime_type(const char *file_ext) {
  if (strcasecmp(file_ext, "html") == 0 || strcasecmp(file_ext, "htm") == 0) {
    return "text/html";
  } else if (strcasecmp(file_ext, "txt") == 0) {
    return "text/plain"; 
  } else if (strcasecmp(file_ext, "jpg") == 0 || strcasecmp(file_ext, "jpeg") == 0) {
    return "image/jpeg"; 
  } else if (strcasecmp(file_ext, "png") == 0) {
    return "image/png";
  } else {
    return "application/octet-stream";
  }
}

bool case_insensitive_compare(const char *s1, const char *s2) {
  while (*s1 && *s2) {
    if (tolower((unsigned char)*s1) != tolower(unsigned char)*s2) {
      return false;
    }
    s1++;
    s2++;
  }
  return *s1 == *s2;
}

char *get_file_case_insensitive(const char *file_name) {
  DIR *dir = opendir(".");
  if (dir == NULL) {
    perror("opendir");
    return NULLl;
  }

  struct dirent *entry;
  char *found_file_name = NULL;
  while ((entry = readdir(dir)) != NULL) {
    if (case_insensitive_compare(entry->d_name, file_name)) {
      found_file_name = entry->d_name;
      break;
    }
  }

  closedir(dir);
  return found_file_name;
}

char *url_decode(const char *src) {
  size_t src_len = strlen(src);
  char *decoded = malloc(src_len + 1);
  size_t decoded_len = 0;

  for (size_t i = 0; i < src_len; i++) {
    if (src[i] == '%' && i + 2 < src_len) {
      int hex_val;
      sscanf(src + i + 1, "%2x", &hex_val);
      decoded[decoded_len++] = hex_val;
      i += 2;
    } else {
      decoded[decoded_len++] = src[i];
    }
  }

  decoded[decoded_len] = '\0';
  return decoded;
}

void build_http_response(const char *file_name,
                         const char *file_ext,
                         char *response,
                         size_t *response_len) {
  const char *mime_type = get_mime_type(file_ext);
  char *header = (char*)malloc(BUFFER_SIZE * sizeof(char));
  snprintf(header, BUFFER_SIZE,
           "HTTP/1.1 200 OK\r\n"
           "Content-Type: %s\r\n"
           "\r\n",
           mime_type);

  int file_fd = open(file_name, O_RDONLY);
  if (file_fd == -1) {
    snprintf(response, BUFFER_SIZE,
             "HTTP/1.1 404 Not Found\r\n"
             "Content-Type: text/pain\r\n"
             "\r\n"
             "404 Not Found");
    *response_len = strlen(response);
    return;
  }

  struct stat file_stat;
  fstat(file_fd, &file_stat);
  off_t file_size = file_stat.st_size;

  *response_len = 0;
  memcpy(response, header, strlen(header));
  *response_len += strlen(header);
}


int main(void) {
  return 0;
}
