/*
This is the main database server file. The server runs infinitely and listens for requests made at the read end of the fifo. Then it extracts the parameters from the 
requet,checks which database operation needs to be done and executes it using the 'database_helper_functions'. And finally writes back the response to the fifo.

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "database_helper_functions.h"
#include "database_macros.h"

int rwfm_enabled = 0;

int num_hosts = 0;
HOST * all_hosts = NULL;

int num_user_ids = 0;
USER_ID * all_user_ids = NULL;

int num_group_ids = 0;
GROUP_ID * all_group_ids = NULL;

int num_object_ids = 0;
OBJECT_ID * all_object_ids = NULL;

int num_subject_ids = 0;
SUBJECT_ID * all_subject_ids = NULL;

int num_objects = 0;
OBJECT * all_objects = NULL;

int num_subjects = 0;
SUBJECT * all_subjects = NULL;

int num_socket_connections = 0;
SOCKET_CONNECTION_OBJECT * all_socket_connections = NULL;

int num_pipe_objects = 0;
PIPE_OBJECT * all_pipe_objects = NULL;

int num_fd_maps = 0;
FD_MAP * fd_map = NULL;

int num_pipe_ref_maps = 0;
PIPE_REF_MAP * pipe_ref_map = NULL;

int read_request(char *req) {
    int read_fifo_fd = open(REQUEST_FIFO_PATH, O_RDONLY);
    int ret = read(read_fifo_fd, req, MAX_REQUEST_LENGTH);
    close(read_fifo_fd);
    return ret;
}

int write_response(char *res) {
    int write_fifo_fd = open(RESPONSE_FIFO_PATH, O_WRONLY);
    int ret = write(write_fifo_fd, res, strlen(res)+1);
    close(write_fifo_fd);
    return ret;
}

//Converting space seperated request string into arguments array
int get_args_from_request(char **args, char *req) {
    memset(args, '\0', sizeof(char*) * MAX_REQUEST_LENGTH);
    char *curToken = strtok(req, " ");
    int i;
    for (i = 0; curToken != NULL; i++) {
      args[i] = strdup(curToken);
      curToken = strtok(NULL, " ");
    }

    return i;
}

int do_operation(int operation, char **req_args, int num_args) {
    char response[MAX_REQUEST_LENGTH];

    switch(operation) {

        case IS_RWFM_ENABLED_OP:
        {
            if(num_args!=1)
                return -1;
            sprintf(response, "%d", rwfm_enabled);
            write_response(response);
            break;
        }
        case SET_RWFM_ENABLED_OP:
        {
            if(num_args!=2)
                return -1;
            rwfm_enabled = strtol(req_args[1], NULL, 10);
            sprintf(response, "%d", rwfm_enabled);
            write_response(response);
            break;
        }


        case ADD_HOST_OP:
        {
            if(num_args!=2)
                return -1;
            sprintf(response, "%d", add_host(req_args[1]));
            write_response(response);
            break;
        }
        case GET_HOST_INDEX_OP:
        {
            if(num_args != 2)
                return -1;
            sprintf(response, "%d", get_host_index(req_args[1]));
            write_response(response);
            break;
        }


        case ADD_USER_ID_OP:
        {
            if(num_args != 3)
                return -1;
            USER_ID user_id_add;
            user_id_add.host_id_index = strtol(req_args[1], NULL, 10);
            user_id_add.uid = strtol(req_args[2], NULL, 10);
            sprintf(response, "%d", add_user_id(user_id_add));
            write_response(response);
            break;
        }
        case GET_USER_ID_INDEX_OP:
        {
            if(num_args != 3)
                return -1;
            USER_ID user_id_get;
            user_id_get.host_id_index = strtol(req_args[1], NULL, 10);
            user_id_get.uid = strtol(req_args[2], NULL, 10);
            sprintf(response, "%d", get_user_id_index(user_id_get));
            write_response(response);
            break;
        }
        case GET_NUMBER_OF_USERS_OP:
        {
            if(num_args != 1)
                return -1;
            sprintf(response, "%d", get_number_of_users());
            write_response(response);
            break;
        }


        case ADD_GROUP_ID_OP:
        {
            if(num_args != 4)
                return -1;
            GROUP_ID group_id_add;
            group_id_add.host_id_index = strtol(req_args[1], NULL, 10);
            group_id_add.gid = strtol(req_args[2], NULL, 10);
            group_id_add.members = strtoull(req_args[3], NULL, 16);
            sprintf(response, "%d", add_group_id(group_id_add));
            write_response(response);
            break;
        }
        case GET_GROUP_ID_INDEX_OP:
        {
            if(num_args != 3)
                return -1;
            GROUP_ID group_id_get;
            group_id_get.host_id_index = strtol(req_args[1], NULL, 10);
            group_id_get.gid = strtol(req_args[2], NULL, 10);
            sprintf(response, "%d", get_group_id_index(group_id_get));
            write_response(response);
            break;
        }
        case GET_MEMBERS_FROM_GROUP_ID_OP:
        {
            if(num_args != 2)
                return -1;
            int group_id_index = strtol(req_args[1], NULL, 10);
            sprintf(response, "%llx", get_members_from_group_id(group_id_index));
            write_response(response);
            break;
        }


        case ADD_OBJECT_ID_OP:
        {
            if(num_args != 4)
                return -1;
            OBJECT_ID object_id_add;
            object_id_add.host_id_index = strtol(req_args[1], NULL, 10);
            object_id_add.device_id = strtoul(req_args[2], NULL, 10);
            object_id_add.inode_number = strtoul(req_args[3], NULL, 10);
            sprintf(response, "%d", add_object_id(object_id_add));
            write_response(response);
            break;
        }
        case GET_OBJECT_ID_INDEX_OP:
        {
            if(num_args != 4)
                return -1;
            OBJECT_ID object_id_get;
            object_id_get.host_id_index = strtol(req_args[1], NULL, 10);
            object_id_get.device_id = strtoul(req_args[2], NULL, 10);
            object_id_get.inode_number = strtoul(req_args[3], NULL, 10);
            sprintf(response, "%d", get_object_id_index(object_id_get));
            write_response(response);
            break;
        }
		case ADD_OBJECT_OP:
        {
            if(num_args != 5)
                return -1;
            OBJECT object_add;
            object_add.obj_id_index = strtol(req_args[1], NULL, 10);
            object_add.owner = strtol(req_args[2], NULL, 10);
            object_add.readers = strtoull(req_args[3], NULL, 16);
            object_add.writers = strtoull(req_args[4], NULL, 16);
            sprintf(response, "%d", add_object(object_add));
            write_response(response);
            break;
        }
        case GET_OBJECT_OP:
        {
            if(num_args != 2)
                return -1;
            int obj_id_index = strtol(req_args[1], NULL, 10);
            int obj_index = get_object_from_obj_id_index(obj_id_index);
            if(obj_index == -1)
                return -2;
            OBJECT obj = all_objects[obj_index];
            sprintf(response, "%u %llx %llx",obj.owner,obj.readers,obj.writers);
            write_response(response);
            break;
        }
        case UPDATE_OBJECT_LABEL_OP:
        {
            if(num_args != 4)
                return -1;
            int update_obj_id = strtol(req_args[1], NULL, 10);
            USER_SET new_obj_readers = strtoull(req_args[2], NULL, 16);
            USER_SET new_obj_writers = strtoull(req_args[3], NULL, 16);
            sprintf(response, "%d", update_object_label(update_obj_id, new_obj_readers, new_obj_writers));
            write_response(response);
            break;
        }


        case ADD_SUBJECT_ID_OP:
        {
            if(num_args != 4)
                return -1;
            SUBJECT_ID subject_id_add;
            subject_id_add.host_id_index = strtol(req_args[1], NULL, 10);
            subject_id_add.uid = strtol(req_args[2], NULL, 10);
            subject_id_add.pid = strtol(req_args[3], NULL, 10);
            sprintf(response, "%d", add_subject_id(subject_id_add));
            write_response(response);
            break;
        }
        case GET_SUBJECT_ID_INDEX_OP:
        {
            if(num_args != 4)
                return -1;
            SUBJECT_ID subject_id_get;
            subject_id_get.host_id_index = strtol(req_args[1], NULL, 10);
            subject_id_get.uid = strtol(req_args[2], NULL, 10);
            subject_id_get.pid = strtol(req_args[3], NULL, 10);
            sprintf(response, "%d", get_subject_id_index(subject_id_get));
            write_response(response);
            break;
        }
        case ADD_SUBJECT_OP:
        {
            if(num_args != 5)
                return -1;
            SUBJECT subject_add;
            subject_add.sub_id_index = strtol(req_args[1], NULL, 10);
            subject_add.owner = strtol(req_args[2], NULL, 10);
            subject_add.readers = strtoull(req_args[3], NULL, 16);
            subject_add.writers = strtoull(req_args[4], NULL, 16);
            sprintf(response, "%d", add_subject(subject_add));
            write_response(response);
            break;
        }
        case GET_SUBJECT_OP:
        {
            if(num_args != 2)
                return -1;
            int sub_id_index = strtol(req_args[1], NULL, 10);
            int sub_index = get_subject_from_sub_id_index(sub_id_index);
            if(sub_index == -1)
                return -2;
            SUBJECT sub = all_subjects[sub_index];
            sprintf(response, "%u %llx %llx",sub.owner,sub.readers,sub.writers);
            write_response(response);
            break;
        }
        case UPDATE_SUBJECT_LABEL_OP:
        {
            if(num_args != 4)
                return -1;
            int update_sub_id = strtol(req_args[1], NULL, 10);
            USER_SET new_sub_readers = strtoull(req_args[2], NULL, 16);
            USER_SET new_sub_writers = strtoull(req_args[3], NULL, 16);
            sprintf(response, "%d", update_subject_label(update_sub_id, new_sub_readers, new_sub_writers));
            write_response(response);
            break;
        }


		case ADD_CONNECTION_OP:
		{
			if(num_args != 9)
                return -1;
            SOCKET_CONNECTION_OBJECT connection;
			connection.src.ip = strtoul(req_args[1], NULL, 10);
			connection.src.port = strtol(req_args[2], NULL, 10);
			connection.dstn.ip = strtoul(req_args[3], NULL, 10);
			connection.dstn.port = strtol(req_args[4], NULL, 10);
			connection.num_peers = strtol(req_args[5], NULL, 10);
            connection.peer_ids[0] = strtol(req_args[6], NULL, 10);
            connection.peer_ids[1] = -1;
            connection.readers = strtoull(req_args[7], NULL, 16);
            connection.writers = strtoull(req_args[8], NULL, 16);
            sprintf(response, "%d", add_connection(connection));
            write_response(response);
            break;
		}
		case GET_CONNECTION_INDEX_OP:
		{
			if(num_args != 5)
                return -1;
			ADDRESS src, dstn;
			src.ip = strtoul(req_args[1], NULL, 10);
            src.port = strtoul(req_args[2], NULL, 10);
			dstn.ip = strtoul(req_args[3], NULL, 10);
            dstn.port = strtoul(req_args[4], NULL, 10);
            sprintf(response, "%d", get_connection_index(src, dstn));
            write_response(response);
            break;
		}
		case GET_CONNECTION_OP:
        {
            if(num_args != 2)
                return -1;
            int connection_index = strtol(req_args[1], NULL, 10);
            SOCKET_CONNECTION_OBJECT connection = all_socket_connections[connection_index];
            sprintf(response, "%llx %llx", connection.readers, connection.writers);
            write_response(response);
            break;
        }
		case UPDATE_CONNECTION_LABEL_OP:
        {
            if(num_args != 4)
                return -1;
            int connection_index = strtol(req_args[1], NULL, 10);
            USER_SET readers = strtoull(req_args[2], NULL, 16);
            USER_SET writers = strtoull(req_args[3], NULL, 16);
            sprintf(response, "%d", update_connection_label(connection_index, readers, writers));
            write_response(response);
            break;
        }
        case REMOVE_PEER_FROM_CONNECTION_OP:
        {
            if(num_args != 6)
                return -1;
			ADDRESS src, dstn;
            src.ip = strtoul(req_args[1], NULL, 10);
            src.port = strtoul(req_args[2], NULL, 10);
			dstn.ip = strtoul(req_args[3], NULL, 10);
            dstn.port = strtoul(req_args[4], NULL, 10);
			int peer_id = strtol(req_args[5], NULL, 10);
            sprintf(response, "%d", remove_peer_from_connection(src, dstn, peer_id));
            write_response(response);
            break;
        }


		case ADD_PIPE_OP:
		{
			if(num_args != 7)
                return -1;
			PIPE_OBJECT pipe_obj;
            pipe_obj.host_id_index = strtol(req_args[1], NULL, 10);
            pipe_obj.device_id = strtoul(req_args[2], NULL, 10);
			pipe_obj.inode_number = strtoul(req_args[3], NULL, 10);
            pipe_obj.pipe_ref_count = strtol(req_args[4], NULL, 10);
			pipe_obj.readers = strtoull(req_args[5], NULL, 16);
            pipe_obj.writers = strtoull(req_args[6], NULL, 16);
            sprintf(response, "%d", add_new_pipe(pipe_obj));
            write_response(response);
            break;
		}
		case INCREASE_PIPE_REF_COUNT_OP:
		{
			if(num_args != 2)
                return -1;
            int pipe_id = strtol(req_args[1], NULL, 10);
            sprintf(response, "%d", increase_pipe_ref_count(pipe_id));
            write_response(response);
            break;
		}
		case GET_PIPE_INDEX_OP:
		{
			if(num_args != 4)
                return -1;
            int host_id_index = strtol(req_args[1], NULL, 10);
            int device_id = strtoul(req_args[2], NULL, 10);
			int inode_number = strtoul(req_args[3], NULL, 10);
            sprintf(response, "%d", get_pipe_index(host_id_index, device_id, inode_number));
            write_response(response);
            break;
		}
		case GET_PIPE_OP:
		{
            if(num_args != 2)
                return -1;
            int pipe_id = strtol(req_args[1], NULL, 10);
            PIPE_OBJECT pipe_obj = all_pipe_objects[pipe_id];
            sprintf(response, "%d %llx %llx", pipe_obj.pipe_ref_count, pipe_obj.readers, pipe_obj.writers);
            write_response(response);
            break;
        }
		case UPDATE_PIPE_LABEL_OP:
		{
			if(num_args != 4)
                return -1;
            int pipe_id = strtol(req_args[1], NULL, 10);
            USER_SET readers = strtoull(req_args[2], NULL, 16);
            USER_SET writers = strtoull(req_args[3], NULL, 16);
            sprintf(response, "%d", update_pipe_label(pipe_id, readers, writers));
            write_response(response);
            break;
		}
		case REMOVE_PIPE_OP:
		{
            if(num_args != 4)
                return -1;
            int host_id_index = strtol(req_args[1], NULL, 10);
            int device_id = strtoul(req_args[2], NULL, 10);
			int inode_number = strtoul(req_args[3], NULL, 10);
            sprintf(response, "%d", remove_pipe(host_id_index, device_id, inode_number));
            write_response(response);
            break;
        }


        case ADD_NEW_FD_MAPPING_OP:
        {
            if(num_args != 4)
                return -1;
            FD_MAP fd_map;
            fd_map.sub_id_index = strtol(req_args[1], NULL, 10);
            fd_map.obj_id_index = strtol(req_args[2], NULL, 10);
            fd_map.fd = strtol(req_args[3], NULL, 10);
            sprintf(response, "%d", add_new_mapping(fd_map));
            write_response(response);
            break;
        }
        case GET_OBJ_ID_FROM_FD_MAP_OP:
        {
            if(num_args != 3)
                return -1;
            int sub_id_ind = strtol(req_args[1], NULL, 10);
            int fd_obj = strtol(req_args[2], NULL, 10);
            sprintf(response, "%d", get_obj_id_index_from_sub_id_and_fd(sub_id_ind, fd_obj));
            write_response(response);
            break;
        }
        case GET_SUB_ID_FROM_FD_MAP_OP:
        {
            if(num_args != 3)
                return -1;
            int obj_id_ind = strtol(req_args[1], NULL, 10);
            int fd_sub = strtol(req_args[2], NULL, 10);
            sprintf(response, "%d", get_obj_id_index_from_sub_id_and_fd(obj_id_ind, fd_sub));
            write_response(response);
            break;
        }
        case REMOVE_FD_MAPPING_OP:
        {
            if(num_args != 3)
                return -1;
            int sub_id_ind = strtol(req_args[1], NULL, 10);
            int fd_obj = strtol(req_args[2], NULL, 10);
            sprintf(response, "%d", remove_fd_map(sub_id_ind, fd_obj));
            write_response(response);
            break;
        }


		case ADD_NEW_PIPE_REF_MAPPING_OP:
        {
            if(num_args != 4)
                return -1;
            PIPE_REF_MAP pipe_map;
            pipe_map.sub_id_index = strtol(req_args[1], NULL, 10);
            pipe_map.pipe_index = strtol(req_args[2], NULL, 10);
            pipe_map.ref_count = strtol(req_args[3], NULL, 10);
            sprintf(response, "%d", add_new_pipe_mapping(pipe_map));
            write_response(response);
            break;
        }
		case INCREMENT_PIPE_MAPPING_REF_COUNT_OP:
        {
            if(num_args != 3)
                return -1;
            int sub_id_index = strtol(req_args[1], NULL, 10);
            int pipe_index = strtol(req_args[2], NULL, 10);
            sprintf(response, "%d", increment_pipe_mapping_ref_count(sub_id_index, pipe_index));
            write_response(response);
            break;
        }
		case DECREMENT_PIPE_MAPPING_REF_COUNT_OP:
        {
            if(num_args != 3)
                return -1;
            int sub_id_index = strtol(req_args[1], NULL, 10);
            int pipe_index = strtol(req_args[2], NULL, 10);
            sprintf(response, "%d", decrement_pipe_mapping_ref_count(sub_id_index, pipe_index));
            write_response(response);
            break;
        }


        case COPY_SUBJECT_FDS_OP:
        {
            if(num_args != 3)
                return -1;
            int src_sub_id_index = strtol(req_args[1], NULL, 10);
            int dstn_sub_id_index = strtol(req_args[2], NULL, 10);
            sprintf(response, "%d", copy_subject_info(src_sub_id_index, dstn_sub_id_index));
            write_response(response);
            break;
        }


        default:
            return -2;
    }

	printf("Operation response %d : %s\n",operation,response);

    return 0;
}

int start_server() {
    while(1) {
        char request[MAX_REQUEST_LENGTH];
        read_request(request);
        printf("Received request:%s\n",request);
        char **req_args = (char**)malloc(MAX_REQUEST_LENGTH * sizeof(char*));
        int num_args = get_args_from_request(req_args, request);
        int operation = strtol(req_args[0], NULL, 10);
        int ret = do_operation(operation, req_args, num_args);
        printf("Operation result:%d\n\n",ret);
        if(ret!=0) {
			printf("Stopping database server!\n\n");
            return ret;
		}
    }
}

int main() {
    start_server();

    return 0;
}
