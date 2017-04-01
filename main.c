#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <math.h>

#define Malloc(nb,type) (type *) malloc(nb*sizeof(type));

typedef enum{false,true} boolean;

typedef struct{
    float x;
    float y;
    boolean intersection;
} Point;

typedef struct{
    Point start;
    Point end;
    float a;
    float b;
} Segment;

typedef struct lpoint{
    Point p;
    struct lpoint *next;
} lpoint;

typedef struct lsegment{
    Segment s;
    struct lsegment* next;
} lsegment;



void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    // Here p is the address to the pixel we want to set
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}


lpoint* ajouter_point(lpoint *tete,Point p)
{
    lpoint *nvPoint = Malloc(1,lpoint);
    nvPoint->p = p;
    nvPoint->next = tete;
    tete = nvPoint;
    return tete;
}

void afficher_list(lpoint *tete)
{

    while(tete!=NULL)
    {
        fprintf(stdout,"%f %f",tete->p.x,tete->p.y);
        if(tete->p.intersection == true)
            fprintf(stdout, " true\n");
        else
            fprintf(stdout, "false\n");
        tete = tete->next;
    }
}
void afficher_list_seg(lsegment *teteSeg)
{
    int i=1;
    while(teteSeg!=NULL)
    {
        fprintf(stdout,"seg %d: (%f,%f) (%f,%f)\n",i,teteSeg->s.start.x,teteSeg->s.start.y,teteSeg->s.end.x,teteSeg->s.end.y);
        teteSeg = teteSeg->next;
        i++;
    }
}


lsegment* ajouter_segement(lsegment* teteSegment,Segment s)
{
    lsegment *nvSeg = Malloc(1,lsegment);
    nvSeg->s = s;
    nvSeg->next = teteSegment;
    teteSegment = nvSeg;
    return teteSegment;
}

lsegment *generer_segments(lpoint *tete)
{
    lsegment* teteSegment=NULL;
    Segment tempSeg;
    lpoint *temp1,*temp2;
    temp1=tete;

    while(temp1 != NULL)
    {
            temp2 = temp1->next;
            while(temp2 != NULL)
            {
                tempSeg.start = temp1->p;
                tempSeg.end = temp2->p;
                if(tempSeg.end.x - tempSeg.start.x != 0)
                    tempSeg.a = (tempSeg.start.y - tempSeg.end.y) / (tempSeg.start.x - tempSeg.end.x);
                else
                    tempSeg.a =  INFINITY;
                tempSeg.b = tempSeg.start.y- tempSeg.a*tempSeg.start.x;

                teteSegment = ajouter_segement(teteSegment,tempSeg);
                temp2 = temp2->next;
            }

            temp1 = temp1->next;
    }

    return teteSegment;
}


void draw_segment(SDL_Surface *screen,Segment seg,Uint32 couleur)
{

        float a;
        Point p=seg.start;
        a = (seg.end.y-seg.start.y)/(seg.end.x-seg.start.x);
        SDL_LockSurface(screen);
        while(p.x != seg.end.x && p.y != seg.end.y )
        {
            setPixel(screen,p.x,p.y,couleur);
            p.x+=0.5;
            p.y+=a;
        }
        SDL_UnlockSurface(screen);
}

void Draw_Lines(SDL_Surface *screen ,Segment seg,Uint32 couleur)
{
    int x1 = seg.start.x;
    int x2 = seg.end.x;
    int y1 = seg.start.y;
    int y2 = seg.end.y;
    double i;
    double x = x2 - x1;
    double y = y2 - y1;
    double length = sqrt( x*x + y*y );

    double addx = x / length;
    double addy = y / length;

    x = x1;
    y = y1;

    for( i = 0; i < length; i += 1)
    {

      setPixel(screen, (int)x, (int)y,couleur );
      x += addx;
      y += addy;
    }
}

void repere(SDL_Surface *surface,Uint32 couleur)
{
    int i,j=50;
    int xo=300,yo=300;
    SDL_LockSurface(surface);
    for(i=-300;i<300;i++) setPixel(surface,xo+i,yo,couleur);
    for(i=-300;i<300;i++) setPixel(surface,xo,yo+i,couleur);
    while(j<=300){
        for(i=-5;i<=5;i++) setPixel(surface,xo+j,yo+i,couleur);
        for(i=-5;i<=5;i++) setPixel(surface,xo+i,yo-j,couleur);
        for(i=-5;i<=5;i++) setPixel(surface,xo-j,yo+i,couleur);
        for(i=-5;i<=5;i++) setPixel(surface,xo+i,yo+j,couleur);
        j+=50;
    }
    SDL_UnlockSurface(surface);
}

void cercle( int xo, int yo, int R, Uint32 couleur,SDL_Surface *surface)
{
    int x, y, F, F1, F2,newx,newy;
    x=xo; y=yo+R; F=0;
    if (x<600 && x>=0 && y>=0 && y<600) setPixel(surface,x,y,couleur);
    if (x<600 && x>=0 && 2*yo-y<600) setPixel (surface,x,2*yo-y, couleur);
    while( y>yo)
    {
    F1=F+2*(x-xo)+1; F2=F-2*(y-yo)+1;
    if ( abs(F1)<abs(F2)) { x+=1; F=F1;}
    else {y-=1; F=F2;}
    if (x<600 && x>=0 && y>=0 && y<600) setPixel(surface,x,y,couleur);
    newx=2*xo-x ; newy=2*yo-y;
    if (x<600 && x>=0 && newy>=0 && newy<600) setPixel(surface,x, newy,couleur);
    if (newx<600 && newx>=0 && y>=0 && y<600) setPixel(surface, newx,y,couleur);
    if (newx<600 && newx>=0 && newy>=0 && newy<600) setPixel(surface,newx,newy, couleur);
    }
    if (xo+R<600 && xo+R>=0) setPixel(surface,xo+R,yo,couleur);
    if (xo-R<600 && xo-R>=0) setPixel(surface,xo-R,yo, couleur);
}

void cercleplein(int x,int y,int rayon, Uint32 couleur, SDL_Surface *surface)
{

    while(rayon >=0)
    {
        cercle(x ,y ,rayon,couleur,surface);
        rayon--;
    }

}

lpoint* supprimer_points(lpoint *tete)
{
    lpoint* temp=NULL;
    while(tete != NULL)
    {
        temp = tete;
        tete = tete->next;
        free(temp);
    }
    return NULL;
}

lsegment* supprimer_segment(lsegment *tete,Segment s)
{
    lsegment *traverse  = tete;
    lsegment *precedent;

    //si le premier element est valide
    if(traverse != NULL && (traverse->s.start.x != s.start.x || traverse->s.end.x != s.end.x || traverse->s.start.y != s.start.y || traverse->s.end.y != s.end.y))
    {
            tete = tete->next;
            free(traverse);
            return tete;
    }

    traverse = traverse->next;
    precedent = traverse;
    while(traverse!= NULL && (traverse->s.start.x != s.start.x || traverse->s.end.x != s.end.x || traverse->s.start.y != s.start.y || traverse->s.end.y != s.end.y))
    {
        precedent = traverse;
        traverse = traverse->next;
    }


    if(traverse != NULL)
    {
        precedent->next = traverse->next;
        free(traverse);
        return tete;
    }

}

lsegment* supprimer_segments(lsegment *tete)
{
    lsegment* temp=NULL;
    while(tete != NULL)
    {
        temp = tete;
        tete = tete->next;
        free(temp);
    }
    return NULL;
}


    //supprimer les segments inutiles
    //1-mettre une liste des points inclus dans un segment
    //trier les points
    //supprimer tt les segments entre les differents points dans le segment y inclu le start et end du segment initiale
    //ajouter les segemnts entre chaque 2 points adjacent

    //if( (p.x != s.start.x || p.y!=s.start.y) && (p.x != s.end.x || p.y!=s.end.y) && )
lsegment* nettoyer_segments(lsegment* teteSeg,lpoint *tetePoint)
{
    lsegment *seg = teteSeg;
    lpoint *point = NULL;
    lpoint *point_inclus = NULL;
    lpoint *nvPoint = NULL;

    while(seg != NULL)
    {
        point = tetePoint;
        while(point != NULL)
        {

            if(point_appartient_seg(seg,point))
            {
                nvPoint = Malloc(1,lpoint);
                point_inclus = ajouter_point(point_inclus,nvPoint);
            }

            point = point->next;
        }

        SelectionSort(point_inclus, seg);
        //supprimer les segments

        seg = seg->next;
    }

    SelectionSort(point_inclus ,)
}

float distance(Point a, Point b)
{
    return sqrt( (b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y) );
}

void SelectionSort(lpoint *tete,Segment s)
{
	lpoint *start = tete;
	lpoint *traverse;
	lpoint *min;
	lpoint *temp;

	while(start->next)
	{
		min = start;
		traverse = start->next;

		while(traverse)
		{
			/* Find minimum element from array */
			if( distance(min->p,s.start) > distance(traverse->p,s.start) )
			{
				min = traverse;
			}

			traverse = traverse->next;
		}

		temp = start;
		start = min;
		min = temp;

		start = start->next;
	}
}



boolean point_appartient_seg(Segment s,Point p)
{
    return (p.x*s.a+s.b == p.y);
}

float max(float a , float b)
{
    return a>b?a:b;
}

float min(float a , float b)
{
    return a<b?a:b;
}

int adjacent(Segment s1,Segment s2){
     if((s1.start.x==s2.start.x&&s1.start.y==s2.start.y)||(s1.start.x==s2.end.x&&s1.start.y==s2.end.y)
        ||(s1.end.x==s2.start.x&&s1.end.y==s2.start.y)
        ||(s1.end.x==s2.end.x&&s1.end.y==s2.end.y))
        return 1;
    return 0;
}

int inclu(Segment s1,Segment s2,Point rec){

    if(rec.x <= max(s1.start.x , s1.end.x)
       && rec.x >= min(s1.start.x , s1.end.x)
       && rec.x <= max(s2.start.x , s2.end.x)
       && rec.x >= min(s2.start.x , s2.end.x)
       && rec.y <= max(s1.start.y , s1.end.y)
       && rec.y >= min(s1.start.y , s1.end.y)
       && rec.y <= max(s2.start.y , s2.end.y)
       && rec.y >= min(s2.start.y , s2.end.y) )

    return 1;
    return 0;
}

Point* intersection_seg(Segment s1,Segment s2)
{

    if(s1.a == s2.a || adjacent(s1,s2)) return NULL;
    Point* inter = Malloc(1,Point);

    //ici prendre en consideration un a infinie

    inter->x = (s2.b-s1.b)/(s1.a-s2.a);
    inter->y = s1.a*inter->x+s1.b;

    if(!inclu(s1,s2,*inter)) return NULL;
    return inter;
}

lpoint* ajouter_intersection(lpoint* tete, lsegment* teteSegment)
{
    lsegment* temp1=teteSegment,* temp2=NULL;
    Point* ppoint=NULL;
    Point p;
    while(temp1 != NULL)
    {
            temp2 = temp1->next;
            while(temp2 != NULL)
            {

                ppoint = intersection_seg(temp1->s,temp2->s);

                //si il y a une intersection entre les 2 segments
                if(ppoint != NULL)
                {
                    p.intersection = true;
                    p.x = ppoint->x;
                    p.y = ppoint->y;
                    free(ppoint);

                    tete = ajouter_point(tete,p);
                }


                temp2 = temp2->next;
            }

            temp1 = temp1->next;
    }

    return tete;
}

int main(int argc, char **argv)
{

    lpoint *tete = NULL;
    lsegment *teteSeg = NULL;
    //FILE* fichier = NULL;
    /* fichier = fopen("coord.txt","r");
    if(fichier != NULL)
    {

        //recuperation des coordonnees
        while(fscanf(fichier,"%f %f",&temp.x,&temp.y) != EOF)
        {
            temp.intersection = false;
            tete = ajouter_point(tete,temp); //creer la liste des coord
        }

        //generer les segments
        teteSeg = generer_segments(tete);

        fclose(fichier);
    }

    afficher_list(tete);
    afficher_list_seg(teteSeg);

    */


    //INTERFACE GRAPHIQUE
    SDL_Surface *screen = NULL,*start = NULL,*clear=NULL;
    SDL_Rect position;
    SDL_Event event;
    boolean continuer = true;
    Point nvPoint;


    if(SDL_Init(SDL_INIT_VIDEO)==-1)
    {
        fprintf(stderr,"probleme sdl_init %s\n",SDL_GetError());
        exit(EXIT_FAILURE);
    }

    screen = SDL_SetVideoMode(600,600,32,SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,255,255,255));


    repere(screen,SDL_MapRGB(screen->format,0,0,0));

    start = SDL_LoadBMP("start.bmp");
    position.x=510;
    position.y=560;
    SDL_BlitSurface(start,NULL,screen,&position);


    clear = SDL_LoadBMP("new.bmp");
    position.x=560;
    position.y=560;
    SDL_BlitSurface(clear,NULL,screen,&position);

    while(continuer)
    {
        SDL_WaitEvent(&event);

        switch(event.type)
        {
            case SDL_QUIT:
                continuer = false;
                break;

            case SDL_MOUSEBUTTONDOWN:


                //clique sur le bouton start
                if(event.button.x >= 510 && event.button.x <= 510 + start->w && event.button.y >= 560 && event.button.y <= 560+start->h)
                {
                    teteSeg = generer_segments(tete);
                    tete = ajouter_intersection(tete,teteSeg);
                    afficher_list(tete);

                     //colorier les intersection
                    lpoint *tempo=tete;
                    while(tempo !=NULL)
                    {
                       if(tempo->p.intersection == true)
                        cercleplein(tempo->p.x,tempo->p.y,4,SDL_MapRGB(screen->format,0,60,201),screen);
                       tempo = tempo->next;
                    }



                    //afficher les segements
                    while(teteSeg != NULL)
                    {
                        Draw_Lines(screen,teteSeg->s,SDL_MapRGB(screen->format,255,0,0));
                        teteSeg = teteSeg->next;
                    }
                }
                else if(event.button.x >= 560 && event.button.x <= 560 + start->w && event.button.y >= 560 && event.button.y <= 560+start->h)
                {
                        tete = supprimer_points(tete);
                        teteSeg = supprimer_segments(teteSeg);

                        //clear screen
                        SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,255,255,255));
                        repere(screen,SDL_MapRGB(screen->format,0,0,0));

                        //position play
                        position.x=510;
                        position.y=560;
                        SDL_BlitSurface(start,NULL,screen,&position);
                        //position new
                        position.x=560;
                        position.y=560;
                        SDL_BlitSurface(clear,NULL,screen,&position);

                } //ajout point
                else
                {
                    nvPoint.x = event.button.x;
                    nvPoint.y = event.button.y;
                    nvPoint.intersection = false;

                    tete = ajouter_point(tete,nvPoint);

                    cercleplein(event.button.x,event.button.y,4,SDL_MapRGB(screen->format,45,45,0),screen);

                }


                break;

            default:
                break;
        }

        SDL_Flip(screen);
    }


    SDL_Quit();
    return 0;
}


