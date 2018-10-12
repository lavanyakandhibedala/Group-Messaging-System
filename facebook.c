#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<conio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct comments
{
	char user[8];
	int likes;
	char comment[116];
}comments;
typedef struct coffset
{
	int indirect_ptr[32];
}coffset;
typedef struct moffset
{
	int indirect_ptr[64];
}moffset;
typedef struct messages
{
	char user[8];
	char message[180];
	int likes, no_of_comments;
	int direct_comment[10];
	int indirect_comment[4];
	int d_indirect_comment;
}messages;
typedef struct groups
{
	char gname[10];
	char desc[50];
	int num_user, num_mgs;
	int direct_mgs[10];
	int indirect_mgs[4];
	int d_indirect_mgs;
}groups;

#define DIRECT 10
#define INDIRECT 4
#define INDIRECT_mgs 64
#define INDIRECT_com 32 
#define IN_INDIRECT 1
#define OFFSET (1024*1024*8)




void start();
void bit_vector_init();
void create_groups();
void add_group(char* gname, char* desc, int n);
void select_group();
char* login();
void select_mgs(char *user, int g);
void write_message(char *user, int g);
void read_mgs(char *user, int g);
void select_message(char *user, int *msgoff,int no_of_msg);
void view_message(char *user, int moff);
void mlike(int moff);
void add_comment(int moff, char *user);
void view_comments(int moff, char *user);
void select_comment(char *user, int *cmnt,int no_of_cmt);
void clike(int coff);
void set_indirect_msg_ptr(int ind_off);
void indirect_msg(int moff, int ind_off);
int byte_vector_write(FILE *fp, int flag);





int main()
{
	int i;
	start();
	select_group();
	_getch();
	return 0;
}


void start()
{
	int i;
	char *user;
	FILE *fp;
	if (fopen("myfile.bin", "r") == NULL)
	{
		fp = fopen("myfile.bin", "wb");
		fseek(fp, 1024 * 1024 * 1024, SEEK_SET);
		fputc('\n', fp);
		fclose(fp);
		bit_vector_init();
		create_groups();
	}
}

void bit_vector_init()
{
	FILE *fp = fopen("myfile.bin", "rb+");
	char *ch = "LK", *b = "1", *c = "0";
	int i;
	fseek(fp, 0, 0);
	fwrite((char*)ch, strlen(ch), 1, fp);
	for (i = 0; i < 10; i++)
		fwrite((char*)b, strlen(b), 1, fp);
	for (i = ftell(fp); i < OFFSET; i++)
		fwrite((char*)c, strlen(c), 1, fp);
}


void create_groups()
{
	add_group("movies", "this is all about movie buff's", 0);
	add_group("entertainment", "this is all about movie buff's", 1);
	add_group("sports", "this is all about movie buff's", 2);
	add_group("technologgy", "this is all about movie buff's", 3);
	add_group("ccartoons", "this is all about movie buff's", 4);
	add_group("comics", "this is all about movie buff's", 5);
	add_group("games", "this is all about movie buff's", 6);
	add_group("books", "this is all about movie buff's", 7);
	add_group("sci-fi", "this is all about movie buff's", 8);
	add_group("news", "this is all about movie buff's", 9);

}


void add_group(char* gname, char* desc, int n)
{
	int i, j, indirect_indx;
	FILE *fp = fopen("myfile.bin", "rb+");
	struct groups new_grp;
	for (i = 0; i < DIRECT; i++)
		new_grp.direct_mgs[i] = -1;
	for (i = 0; i < INDIRECT; i++)
		new_grp.indirect_mgs[i] = -1;
	new_grp.d_indirect_mgs = -1;
	new_grp.num_user = 0;
	new_grp.num_mgs = 0;
	strcpy(new_grp.gname, gname);
	strcpy(new_grp.desc, desc);
	fseek(fp, OFFSET + n * 128, SEEK_SET);
	fwrite(&new_grp, sizeof(struct groups), 1, fp);
	fclose(fp);
	return;
}


void select_group()
{
	int g, m, c;
	char  *user;
	user = login();
	printf("\n\t\t\t%s you have logged in\n\n", user);
	while (1)
	{
		printf("choose one of the following groups:\n1.movies\n2.entertainment\n3.sports\n4.technology\n5.cartoons\n6.comics\n7.games\n8.books\n9.sci-fi\n10.news\n11.logout\n12.exit");
		printf("\n\n\nenter your choice:");
		scanf("%d", &g);
		switch (g)
		{
		case 1:printf("Movies\n");
			break;
		case 2:printf("Entertainment\n");
			break;
		case 3:printf("Sports\n");
			break;
		case 4:printf("Technology\n");
			break;
		case 5:printf("Cartoons\n");
			break;
		case 6:printf("Comics\n");
			break;
		case 7:printf("Games\n");
			break;
		case 8:printf("Books\n");
			break;
		case 9:printf("Sci-Fi\n");
			break;
		case 10:printf("News\n");
			break;
		case 11:select_group();
		case 12:exit(0);
		default:printf("invalid option ");
		}
		if (g < 12)
			select_mgs(user, g);
	}
}


char* login()
{
	int i;
	char *user = (char*)malloc(8* sizeof(char));
	fflush(stdin);
	system("cls");
	printf("\t\tenter user name: ");
	gets(user);
	return user;
}



void select_mgs(char *user, int g)
{
	int m, mgs_address;
	char *mgs;

	while (1)
	{
		system("cls");
		printf("\n1.Write Message\n2.View Message\n3.Exit\n");
		printf("enter your  option:");
		scanf("%d", &m);
		system("cls");
		switch (m)
		{
		case 1:
			write_message(user, g);
			break;
		case 2:read_mgs(user, g);
			break;
		case 3:return;
		default:printf("invalid options");

		}
	}
}



void write_message(char *user, int g)
{
	int index = 0, i = 0, flag = 0, indirect_indx = 0;
	FILE *fp = fopen("myfile.bin", "rb+");
	struct messages new_msg;
	struct groups new_grp;
	for (i = 0; i < DIRECT; i++)
		new_msg.direct_comment[i] = -1;
	for (i = 0; i < INDIRECT; i++)
		new_msg.indirect_comment[i] = -1;
	new_msg.d_indirect_comment = -1;
	new_msg.likes = 0;
	new_msg.no_of_comments = 0;
	strcpy(new_msg.user, user);
	strcpy(new_msg.message, "");
	index = byte_vector_write(fp, 1);
	fseek(fp, OFFSET + index * 128, 0);
	fwrite(&new_msg, sizeof(new_msg), 1, fp);
	fseek(fp, OFFSET + (g - 1) * 128, 0);
	fread(&new_grp, sizeof(struct groups), 1, fp);
	for (i = 0; i < INDIRECT; i++)
	{
		if (new_grp.indirect_mgs[i] == -1)
		{
			indirect_indx = byte_vector_write(fp, 1);
			new_grp.indirect_mgs[i] = OFFSET + indirect_indx * 128;
			set_indirect_msg_ptr(indirect_indx);
		}
	}
	i = 0;
	new_grp.num_mgs += 1;
	new_grp.num_user += 1;
	while (new_grp.direct_mgs[i] != -1 && i<10)
	{
		if (flag == 0)
		{
			i++;
			if (i == 10)
				flag = 1;
		}
	}
	//printf("\n%d\n", ftell(fp));
	if (flag == 0)
		new_grp.direct_mgs[i] = OFFSET + index * 128;

	else
	{
		for (i = 0; i < INDIRECT; i++)
		{
			if (new_grp.indirect_mgs[i] == -1)
				indirect_msg(index, indirect_indx);
		}
	}
	fseek(fp, OFFSET + (g - 1) * 128, 0);
	fwrite(&new_grp, sizeof(new_grp), 1, fp);
	fflush(fp);
	rewind(fp);
	printf("enter the message:");
	fflush(stdin);
	gets(new_msg.message);
	fseek(fp, OFFSET + index * 128, 0);
	fwrite(&new_msg, sizeof(new_msg), 1, fp);
	fclose(fp);
}



void read_mgs(char *user, int g)
{
	int offset, i = 0, *msg = (int)malloc(10 * sizeof(int)), j = 0;
	struct groups view_grp;
	struct messages view_msg;
	struct moffset view_msg_indirect;
	FILE *fp = fopen("myfile.bin", "rb");
	fseek(fp, OFFSET + (g - 1) * 128, 0);
	fread(&view_grp, sizeof(struct groups), 1, fp);
	printf("\n\t\t\t\t\t%s\n", view_grp.gname);
	if (view_grp.direct_mgs[i] == -1)
	{
		printf("\nno messages there\n");
		return;
	}
	while (i != 10)
	{
		if (view_grp.direct_mgs[i] == -1)
			break;
		else
		{
			fseek(fp, view_grp.direct_mgs[i], 0);
			fread(&view_msg, sizeof(struct messages), 1, fp);
			printf("\n%d-> (C%d/L%d) %s : %s",i+1,view_msg.no_of_comments, view_msg.likes, view_msg.user, view_msg.message);
			msg[i] = view_grp.direct_mgs[i];
			i++;
		}
	}
	i = 0;
	while (i != 4)
	{
		//printf("\n%d\n", ftell(fp));
		if (view_grp.indirect_mgs[i] == -1)
			break;
		else
		{
			j = 0;
			//printf("\n%d\n", ftell(fp));
			fseek(fp, view_grp.indirect_mgs[i], 0);
			fread(&view_msg_indirect, sizeof(struct moffset), 1, fp);
			//printf("\n%d\n", ftell(fp));
			while (j<64)
			{
				//printf("\n%d\n", view_msg_indirect.indirect_ptr[j++]);
				if (view_msg_indirect.indirect_ptr[j] == -1)
					break;
				else
				{
					//printf("\n%d\n", ftell(fp));
					fseek(fp, view_msg_indirect.indirect_ptr[j], 0);
					fread(&view_msg, sizeof(struct messages), 1, fp);
					//printf("\n%d\n", ftell(fp));
					printf("\n%d-> (C%d/L%d)  %s : %s ",j+1,view_msg.no_of_comments, view_msg.likes, view_msg.user, view_msg.message);
					msg[i] = view_msg_indirect.indirect_ptr[j];
					//printf("\n%d\n", ftell(fp));
					j++;
				}
			}
			i++;
		}
	}
	select_message(user, msg,view_grp.num_mgs);
}


void select_message(char *user, int *msgoff,int no_of_msg)
{
	int c;
	while (1)
	{
		printf("\n\n\n");
		printf("\n1.enter the message number to view message\n11.exit\nenter the value: ");
		scanf("%d", &c);
		system("cls");
		if(c>no_of_msg&&c<10)
		{
			printf("\nenter valid message no\n");
			continue;
		}
		if (c <= 10)
		{
			view_message(user, msgoff[c - 1]);
			return;
		}
		else if (c == 11)
			return;
		else
			printf("invalid option");
	}
}



void view_message(char *user, int moff)
{
	int c;
	while (1)
	{
		system("cls");
		printf("\n1.like\n2.write comment\n3.view comments\n4.exit\nenter your choice:");
		scanf("%d", &c);
		system("cls");
		switch (c)
		{
		case 1:mlike(moff);
			break;
		case 2:add_comment(moff, user);
			break;
		case 3:view_comments(moff, user);
			break;
		case 4:return;
		default:printf("invalid option");
		}
	}
}


void mlike(int moff)
{
	FILE *fp = fopen("myfile.bin", "rb+");
	struct messages lyk_msg;
	fseek(fp, moff, 0);
	fread(&lyk_msg, sizeof(struct messages), 1, fp);
	lyk_msg.likes += 1;
	fseek(fp, moff, 0);
	fwrite(&lyk_msg, sizeof(struct messages), 1, fp);
	fflush(fp);
	fclose(fp);
}


void add_comment(int moff, char *user)
{
	int index, i, f = 0;
	struct coffset c_indirect;
	struct comments new_cmnt;
	struct messages cmt_msg;
	FILE *fp = fopen("myfile.bin", "rb+");
	strcpy(new_cmnt.comment, "");
	new_cmnt.likes = 0;
	strcpy(new_cmnt.user, user);
	index = byte_vector_write(fp, 0);
	fseek(fp, OFFSET + index * 128, 0);
	fwrite(&new_cmnt, sizeof(new_cmnt), 1, fp);
	fseek(fp, moff, 0);
	fread(&cmt_msg, sizeof(struct messages), 1, fp);
	i = 0;
	//printf("\ncomment=%d\n", cmt_msg.likes);
	while (cmt_msg.direct_comment[i] != -1)
		i++;
	cmt_msg.direct_comment[i] = OFFSET + index * 128;
	cmt_msg.no_of_comments++;
	fseek(fp, moff, 0);
	fwrite(&cmt_msg, sizeof(cmt_msg), 1, fp);
	fflush(fp);
	rewind(fp);
	printf("enter the comment:");
	fflush(stdin);
	gets(new_cmnt.comment);
	fseek(fp, OFFSET + index * 128, 0);
	fwrite(&new_cmnt, sizeof(new_cmnt), 1, fp);
	fclose(fp);
}



void view_comments(int moff, char *user)
{
	int offset, i = 0, *cmnt = (int)malloc(10 * sizeof(int));
	struct comments view_cmnt;
	struct messages view_msg;
	FILE *fp = fopen("myfile.bin", "rb+");
	fseek(fp, moff, 0);
	fread(&view_msg, sizeof(struct messages), 1, fp);
	printf("\n\t\t\tmessage:%s : %s\n\n", view_msg.user, view_msg.message);
	if (view_msg.direct_comment[i] == -1)
	{
		printf("\nno comments there\n");
		return;
	}
	while (i != 10)
	{
		if (view_msg.direct_comment[i] == -1)
			break;
		else
		{
			fseek(fp, view_msg.direct_comment[i], 0);
			fread(&view_cmnt, sizeof(struct comments), 1, fp);
			printf("\n%d-> (L%d)  %s : %s ",i+1,view_cmnt.likes, view_cmnt.user, view_cmnt.comment);
			cmnt[i] = view_msg.direct_comment[i];
			i++;
		}

	}
	select_comment(user, cmnt,view_msg.no_of_comments);
}


void select_comment(char *user, int *cmnt,int no_of_cmt)
{
	int c;
	printf("\n\n\n");
	while (1)
	{
		printf("\n\n1.enter the comment number to like comment\n11.exit\nenter the value: ");
		scanf("%d", &c);
		if(c>no_of_cmt&&c<=10)
		{
		       printf("invalid comment number");
		       continue;
	    }
		switch (c)
		{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:	
		        clike(cmnt[c - 1]);
			    break;
		case 11:return;
		default:printf("invalid option");
		}
	}
}


void clike(int coff)
{
	FILE *fp = fopen("myfile.bin", "rb+");
	struct comments lyk_cmt;
	fseek(fp, coff, 0);
	fread(&lyk_cmt, sizeof(struct comments), 1, fp);
	printf("\n\n%s\n\n",lyk_cmt.user);
	lyk_cmt.likes++;
	fseek(fp, coff, 0);
	fwrite(&lyk_cmt, sizeof(struct comments), 1, fp);
	fflush(fp);
	fclose(fp);
}



int byte_vector_write(FILE *fp, int flag)
{
	char b = "0", *ch, *h = "1";
	int i, k;
	rewind(fp);
	for (i = 0; i < (1024 * 1024 * 8); i++)
	{
		ch = getc(fp);
		if (ch == '0')
			break;
	}
	i -= 2;
	fseek(fp, -1, 1);
	if (flag == 1)
		for (k = 0; k < 2; k++)
			fwrite((char*)h, strlen(h), 1, fp);
	else
		fwrite((char*)h, strlen(h), 1, fp);
	return i;
}




void set_indirect_msg_ptr(int ind_off)
{
	int i = 0;
	struct moffset ind_msg;
	FILE *fp = fopen("myfile.bin", "rb+");
	fseek(fp, OFFSET + ind_off * 128, SEEK_SET);
	for (i = 0; i < 64; i++)
		ind_msg.indirect_ptr[i] = -1;
	fwrite(&ind_msg, sizeof(ind_msg), 1, fp);
	fflush(stdin);
	fclose(fp);
}




void indirect_msg(int moff, int ind_off)
{
	int i = 0;
	struct moffset ind_msg;
	FILE *fp = fopen("myfile.bin", "rb+");
	fseek(fp, OFFSET + ind_off * 128, SEEK_SET);
	//printf("\n%d\n", ftell(fp));
	fread(&ind_msg, sizeof(struct moffset), 1, fp);
	//printf("\n%d\n", ftell(fp));
	while (ind_msg.indirect_ptr[i] != -1)
		i++;
	ind_msg.indirect_ptr[i] = OFFSET + moff;
	fseek(fp, OFFSET + ind_off * 128, SEEK_SET);
	//printf("\n%d\n", ftell(fp));
	fwrite(&ind_msg, sizeof(struct moffset), 1, fp);
	fclose(fp);
	return;


}
