#ifndef standard_h
#define standard_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

char* better_fgets(char *line, int len, FILE *in_file);
char* int_to_char(int Num);
int fsize(FILE *file);

template <class A>
struct lnode
{
	lnode()
	{
		next = NULL;
		prev = NULL;
	}

	~lnode()
	{
		next = NULL;
		prev = NULL;
	}

	A data;

	lnode<A> *next;
	lnode<A> *prev;
};

template <class A>
struct llist
{
	llist()
	{
		init();
	}
	~llist()
	{
		clear();
	}

	lnode<A> *first;
	lnode<A> *last;
	int num_nodes;

	void init()
	{
		first = NULL;
		last = NULL;
		num_nodes = 0;
	}

	void clear()
	{
		if (first != NULL)
		{
			lnode<A> *curr = first, *prev;
			while (curr != NULL)
			{
				prev = curr;
				curr = curr->next;
				delete prev;
				prev = NULL;
			}
		}

		first = NULL;
		last = NULL;
		num_nodes = 0;
	}

	void add(A node)
	{
		lnode<A> *nnode = new lnode<A>;
		nnode->data = node;
		nnode->next = NULL;
		nnode->prev = NULL;
		num_nodes++;
		if (first == NULL)
		{
			first = nnode;
			last = first;
		}
		else
		{
			last->next = nnode;
			nnode->prev = last;
			last = last->next;
		}
	}

	void rem(lnode<A> *pos)
	{
		if (pos == first)
			first = first->next;
		if (pos == last)
			last = last->prev;

		lnode<A> *prev = pos->prev;
		if (prev)
			prev->next = pos->next;
		lnode<A> *next = pos->next;
		if (next)
			next->prev = prev;

		delete pos;
		pos = NULL;
		num_nodes--;
	}

	bool editindex(int index, A nnode)
	{
		lnode<A> *curr = first;
		for (int x = 0; x < index && curr != NULL; x++)
			curr = curr->next;

		if (curr == NULL)
			return false;

		curr->data = nnode;

		return true;
	}

	void editpos(lnode<A> *pos, A nnode)
	{
		pos->data = nnode;
	}

	bool getindex(int index, A *node)
	{
		lnode<A> *curr = first;
		for (int x = 0; x < index && curr != NULL; x++)
			curr = curr->next;

		if (curr == NULL)
		{
			*node = NULL;
			return false;
		}
		*node = curr->data;

		return true;
	}

	void getpos(lnode<A> *pos, A *node)
	{
		*node = pos->data;
	}
};

#endif
