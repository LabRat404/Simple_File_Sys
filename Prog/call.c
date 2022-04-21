#include "call.h"
const char *HD = "HD";

//1155144676 Yeung Tang
//1155144676@link.cuhk.edu.hk
//Functions

inode* read_inode(int fd, int inode_number){
	inode* ip = malloc(sizeof(inode));
	int currpos=lseek(fd, INODE_OFFSET + inode_number * sizeof(inode), SEEK_SET);
	if(currpos<0){
		printf("Error: lseek()\n");
		return NULL;
	}

	//read inode from disk
	int ret = read(fd, ip, sizeof(inode));
	if(ret != sizeof (inode) ){
		printf("Error: read()\n");
		return NULL;
	}
	return ip;
}

superblock* read_sb(int fd){
	superblock* sb = malloc(sizeof(superblock));
	int currpos=lseek(fd, SUPER_BLOCK_OFFSET, SEEK_SET);
	if(currpos<0){
		printf("Error: lseek()\n");
		return NULL;
	}

	//read superblock from disk
	int ret = read(fd, sb, sizeof(superblock));
	if(ret != sizeof (superblock) ){
		printf("Error: read()\n");
		return NULL;
	}
	return sb;
}
//End of Functions


//Start of INODEs
void print_inode_info(inode* ip){
	printf("The inode information: \n");
	printf("[1] inode_number:\t\t%d\n", ip->inode_number);
	printf("[2] creation_time:\t\t%s", ctime(& ip->creation_time));
	printf("[3] file_type:\t\t\t%d\n", ip->file_type);
	printf("[4] file_size:\t\t\t%d\n", ip->file_size);
	printf("[5] block_number:\t\t%d\n", ip->block_number);
	printf("[6] direct_block[0]:\t\t%d\n", ip->direct_block[0]);
	printf("[7] direct_block[1]:\t\t%d\n", ip->direct_block[1]);
	printf("[8] indirect_block:\t\t%d\n", ip->indirect_block);
	printf("[9] sub_file_number:\t\t%d\n", ip->sub_file_number);
}

void print_inode_region(int fd, int inode_number){
	printf("The inode region on disk (Hexadecimal):\n");
	unsigned int buf[sizeof(inode) / 4];
	int currpos = lseek(fd, INODE_OFFSET + inode_number * sizeof(inode), SEEK_SET);
	read(fd, buf, sizeof(inode));
	int i;
	for (i = 0; i < sizeof(inode) / 4; i++){
		printf("[%d] %04x\n", i+1, buf[i]);
	}
}

void print_directory_mappings(int fd, int inode_number)
{
	inode* ip;
	ip = read_inode(fd, inode_number);
	if(ip->file_type != DIR)
	{
		printf("Wrong path!\n");
		return;
	}

	DIR_NODE* p_block = (DIR_NODE* )malloc(BLOCK_SIZE);
	// Consider that SFS only supports at most 100 inodes so that only direct_blk[0] will be used,
	// the implementation is much easier
	int block_number = ip->direct_block[0];
	int currpos=lseek(fd, DATA_BLOCK_OFFSET + block_number * BLOCK_SIZE, SEEK_SET);
	read(fd, p_block, BLOCK_SIZE);

	int file_idx = 0;
	printf("file_name \t inode_number\n");
	for(file_idx = 0; file_idx < ip->sub_file_number; file_idx++)
	{
		printf("%s \t\t %d\n", p_block[file_idx].file_name, p_block[file_idx].inode_number);
	}
	free(p_block);
}
//END of INODES

//Start of SuperBLock

void print_sb_info(superblock* sb){
	printf("The superblock information:\n");
	printf("[1] inode_offset:\t\t%d\n", sb->inode_offset);
	printf("[2] data_block_offset:\t\t%d\n", sb->data_block_offset);
	printf("[3] max_inode:\t\t\t%d\n", sb->max_inode);
	printf("[4] max_data_block:\t\t%d\n", sb->max_data_block);
	printf("[5] next_available_inode:\t%d\n", sb->next_available_inode);
	printf("[6] next_available_data_block:\t%d\n", sb->next_available_data_block);
	printf("[7] block_size:\t\t\t%d\n", sb->block_size);
}

void print_sb_region(int fd){
	printf("The superblock region on disk (Hexadecimal):\n");
	unsigned int buf[sizeof(superblock) / 4];
	int currpos=lseek(fd, SUPER_BLOCK_OFFSET, SEEK_SET);
	read(fd, buf, sizeof(superblock));
	int i;
	for (i = 0; i < sizeof(superblock) / 4; i++){
		printf("[%d] %04x\n", i+1, buf[i]);
	}
}
//END of SuperBlock
//Self-Made function
int next_inode_num(int fd, int i_number, char *dir_name){
    int next_inum = -1; //if returns -1 = error
    inode *in;
    in = read_inode(fd, i_number);
    if(in==NULL)
        return -1;

    DIR_NODE *data_block = (DIR_NODE*)malloc(BLOCK_SIZE);
    int block_num = in->direct_block[0];
    int pos = lseek(fd, DATA_BLOCK_OFFSET+block_num*BLOCK_SIZE, SEEK_SET);
    read(fd, data_block, BLOCK_SIZE);
   

    //Find out the right dir/file i num
    for(int i=0; i<in->sub_file_number; i++)
         if(strcmp(data_block[i].file_name, dir_name)==0)
        {
			next_inum = data_block[i].inode_number;
			break;
}


    free(data_block);
    free(in);
    return next_inum;
}
 ////printf("\n dir_name is :%s\n", dir_name);

	 //printf("\ncurrent inum is %d\n",inode_number );
    //printf("hihi nibba: %d\n", inode_number);
int open_t(char *pathname)
{
	int inode_number =0;
	// write your code here.
	int file = open("./HD",O_RDONLY);
	inode *rf = read_inode(file,0); //root
	//printf("first inode 1 info is wtf? : %ld\n", rf->creation_time);
	if(rf==NULL)
		return -1;

	char *directory_name = strtok(pathname,"/");
	while(directory_name!=NULL)
	{
		inode_number = next_inode_num(file, inode_number, directory_name);
		directory_name = strtok(NULL,"/");
		if(inode_number<0){
		printf("Wrong path/File not found");
			break;
}
	}
	free(directory_name);
	free(rf);
	close(file);
	return inode_number;
}


int read_t(int inode_number, int offset, void *buf, int count)
{
	if(inode_number < 0){
		printf("Invaild Inode Number");
		return -1;
	}

	int read_bytes = 0;
	// write your code here.
	
	int file = open("./HD",O_RDONLY);
	if(file<0){
		printf("No Such File/File Open Error");
		return -1;
	}

	inode* ip = read_inode(file, inode_number);
	if(ip == NULL || ip->file_type ==1){
		printf("No Inode Found/Wrong Inode");
		return -1;
	}

	if(offset >= BLOCK_SIZE*ip->block_number){
		printf("Invalid Offset");
		return -1;
	}

	if((offset+count)>= ip->file_size){
		count = ip->file_size-offset;
		if(count<0)
			count=0;
		
	}

	int starting_block = offset/BLOCK_SIZE;
	int ending_block = (offset+count-1)/BLOCK_SIZE;
	int starting_block_offset = offset%BLOCK_SIZE;
	int ending_block_offset = (offset+count-1)%BLOCK_SIZE;
	int current_pos = 0;

	for(int i = starting_block; i <= ending_block; i++){
		int indirect_blocks = 0;
		int read_size = BLOCK_SIZE;
		if(i == starting_block){
			starting_block_offset = offset%BLOCK_SIZE;
			read_size = read_size - starting_block_offset;
		}
		else
			starting_block_offset = 0;
		
		if(i<2)
			current_pos = lseek(file, DATA_BLOCK_OFFSET + BLOCK_SIZE*ip->direct_block[i] + starting_block_offset, SEEK_SET);
		else{
			lseek(file, DATA_BLOCK_OFFSET + BLOCK_SIZE*ip->indirect_block + (i-2)*sizeof(int), SEEK_SET);
			read(file, &indirect_blocks, sizeof(int));
			lseek(file, DATA_BLOCK_OFFSET + BLOCK_SIZE*indirect_blocks + starting_block_offset, SEEK_SET);
		}

		if(i == ending_block){
			ending_block_offset = (offset+count-1)%BLOCK_SIZE;
			read_size = ending_block_offset - starting_block_offset + 1;
		}else
			ending_block_offset = 0;
		

		read(file, (char*)buf + read_bytes, read_size);
		read_bytes += read_size;

	}

	free(ip);
	close(file);
	return read_bytes;
}

// you are allowed to create any auxiliary functions that can help your implementation. But only "open_t()" and "read_t()" are allowed to call these auxiliary functions.