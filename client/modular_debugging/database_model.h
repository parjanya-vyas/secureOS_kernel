#ifndef _DB_MODEL_
#define _DB_MODEL_

typedef unsigned long long int USER_SET;
typedef char * HOST;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef struct user_id {
    uint uid;
    uint host_id_index;
} USER_ID;

typedef struct group_id {
    uint gid;
    uint host_id_index;
    USER_SET members;
} GROUP_ID;

typedef struct object_id {
    uint device_id;
    uint inode_number;
    int host_id_index;
} OBJECT_ID;

typedef struct object {
    uint obj_id_index;
    uint owner;//index of the user id in ALL_UID array
    USER_SET readers;//the bits in places where uid are present from ALL_UID are 1
    USER_SET writers;//the bits in places where uid are present from ALL_UID are 1
} OBJECT;

typedef struct subject_id {
    uint uid;
    uint pid;
    int host_id_index;
} SUBJECT_ID;

typedef struct subject {
    uint sub_id_index;
    uint owner;
    USER_SET readers;
    USER_SET writers;
} SUBJECT;

typedef struct socket_object {
	uint sub_id_index;
	uint sock_fd;
	ulong ip;
	uint port;
    uint owner;
    USER_SET readers;
    USER_SET writers; 
} SOCKET_OBJECT;

typedef struct file_descriptor_map {
    uint sub_id_index;
    uint obj_id_index;
    uint fd;
} FD_MAP;

typedef struct connection_map {
	uint sock_index_1;
	uint sock_index_2;
} CONNECTION_MAP;

#endif
