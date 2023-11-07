Assumption
We were told to do memory management,and in the video,it was mentioned that need not be a pagesize for munmap,hence two varieties were made,one being with munmapping excess spaace and the other dynamic allocation.
Due to this assumptions multiple codes were made ready.

Mems init-the head was initialised by mapping here along with pointers that were needed for dynamic allocation and the list of pointers needed.

Malloc-here first the code iterates through the linked list to find a hole of said space.If found the new node gets added after the subnode with required space and becomes a hole and the said subnode becomes a process.
If not found a new main node is created with adequate page size and nodes that fit the criteria for the malloc.the virtual memory address is got by adding the size to the previous nodes virtual memory

mems get-the nodes contain a offset and this offset is found by iterating through the code to find the said virtual address and the offset is added to this

mems free-similar iteration and converts the process to free in required node.checks previous and next nodes if free to combine the nodes.

print stats-iteration to print

mems finish-the main node stores each mapping hence we iterate through them to delete it and delete the list through the head and linked list size.
