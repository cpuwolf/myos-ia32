/* 
*	written by wei shuai
*	2004/8/31
*/

struct free_list
{
	struct free_list * next; 
};


#define LIST_HEAD(name) static struct free_list * name;

inline static void init_slot(struct free_list ** head)
{
	struct free_list * hp;
	/*put all mem_block into free slot list*/
	for(hp=&mem_block[0];hp<&mem_block[NR_HOLE];hp++)
		hp->next=hp+1;
	*head=mem_block;
	mem_block[NR_HOLE-1].next=0;	
}
static inline void alloc_slot(struct free_list ** head,struct free_list ** ret)	/*like malloc,but it alloc fixed size*/
{
	if(*head!=0)
	{
		*ret=*head;
		*head=*ret->next;
	}else *ret=0;
}
static inline void free_slot(struct free_list ** head,struct free_list * h)	/*like free*/
{
	h->next=*head;	/*insert into head of the free list */
	*head=h;
}

