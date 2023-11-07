Assumption
Sir mentions in the video that the munmap in size of Page size is unncessesary but this hasnt been updated in the docs.We have made 2 codes one focusing on memory management by munmapping each time we mmap a node
and the other code being dynamic allocation.

In mems init we intialise the starting virtualise address and the head of the linked list

In mems malloc we go through every subnode searching for adequate freesize.If found it adds a node in between that node and the next node if it exists.It changes the status of the node to Process and new one to Hole with size of size of node - the mems_malloced size.

Mems finish we itererate through the main node where we have the starting virtual memory and offset stored,so we munmap the allocations from there.While iterating we unmap the nodes memory along with it.

Mems printstats-variables initialised for printing the values,we iterate though the nodes for the said values and save it to variables for printing

Mems get-similar to mems finish but using the offset and the virtual address belong to a specific node we return the physical address 

Mems free-same as mems get but we also iterate though the subnode to find the specific sub node where this memory exists to free that node

