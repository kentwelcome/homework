#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef struct vertex{
        double x;
        double y;
        double z;
        struct vertex *next;        
} Vertex;

typedef struct trangle{
        double max_z;
        Vertex *node0;
        Vertex *node1;
        Vertex *node2;
        struct trangle *next;
} Trangle;
typedef struct {
        int R;
        int G;
        int B;        
} buffer_struct;



void draw_line( double x0, double y0, double z0 , double x1, double y1, double z1);
void draw_pixel( int x , int y , int R , int G ,int B );
void draw_triangle(double x0, double y0, double z0 , double x1, double y1, double z1, double x2, double y2, double z2);
void swap_C( Vertex *var1 , Vertex *var2 );
void draw_scan_Y_line(int x0 , int x1 , int y , int R , int G ,int B );
void Output_ppm();
void init_screenbuffer();
void insert_vertex_point(double x , double y , double z);
void free_Vertex();
void free_Trangle();
void Normalization(double max_x , double max_y , double min_x , double min_y , double max_z ,double min_z);
int  read_file_and_draw();
void insert_trangle(Vertex *node0 , Vertex *node1 , Vertex *node2 );
//void sort_trangle_node();
void draw_sort_trangle(char *filename);
int  color_set(double z0 ,double z1 , double z2 );
Vertex *search_vertex( int index );
Trangle *search_min_z();

buffer_struct screen_buffer[600][800];
Vertex *start_vertex_point;
Vertex *end_vertex_point;
Trangle *start_node;
Trangle *end_node;
int trangle_number;
int RAND_COLOR;
int SORT = 0;


int main()
{
    //int x;
    RAND_COLOR = 0;
    start_vertex_point = NULL;
    end_vertex_point = NULL;
    start_node = NULL;
    end_node = NULL;
    srand(time(NULL));
    init_screenbuffer();
    read_file_and_draw();
    Output_ppm();
    if ( SORT != 0 )
    {
         RAND_COLOR = 1;
         init_screenbuffer();
         draw_sort_trangle("Sort_with_z.ppm");
    }
    free_Vertex();
    free_Trangle();
    return 0;
}

void draw_line( double x0, double y0, double z0 , double x1, double y1, double z1)
{
     double m;
     double Del_x , Del_y;
     double C;
     //double a , b , c;                   // mid point algo a = Del_y , b = -Del_x , c = B*Del_x
     int i,run_time;
     int R,G,B;
     R = rand()%256;
     G = rand()%256;
     B = rand()%256;
          
     Del_x = x0 - x1;
     Del_y = y0 - y1;
     //a = Del_y;
     //b = -Del_x;
     
     if ( fabs(Del_y) <= fabs( Del_x ) )  // 1 >= m >= -1  x 
     {
         if ( Del_x != 0) 
             m = (Del_y ) / ( Del_x );      // y = mx + C
         else
             m = 1;
         C = y0 - m * x0 ;
         //c = Del_x * C;                   // c = B*Del_x
         
         if ( x0 > x1 )
         {
            i = (int)x1;
            run_time = x0;
         }
         else
         {
            i = (int)x0 ;
            run_time = (int)x1;
         }
         y0 = m * i + C;
         
         while(  i < run_time )
         {
             draw_pixel( i , round(y0), R , G , B );
             i++;
             y0 = y0 + m;
         }
     }
     else                                  //  m > 1 , m < -1
     {
         if ( Del_y != 0) 
             m = ( Del_x ) / ( Del_y );        // x = my + C
         else
             m = 1;
         C = x0 - m * y0 ;
         //c = Del_y * C;                   // c = B*Del_x
         
         if (y0 > y1)
         {
            i = (int)y1;
            run_time = (int)y0;
         }
         else
         {
            i = (int)y0 ;
            run_time = (int)y1;          
         }
         

         x0 = m * i + C;
         
         while(  i < run_time )
         {
             draw_pixel( round(x0) , i , R , G , B );
             i++;
             x0 = x0 + m;
         }         
     }
}

void draw_triangle(double x0, double y0, double z0 , double x1, double y1, double z1, double x2, double y2, double z2)
{
     Vertex C[3];                    // point
     double m01 , m02 , m12;         //  Y = mX + C
     double Del_x01 , Del_y01 , Del_x02 , Del_y02 , Del_x12 , Del_y12 ;
     double C01 , C02 , C12;         //  
     int R , G , B;
     int i  , j;
     int color;
     // randen color
     if ( RAND_COLOR == 0 )
     {
        R = rand()%256;
        G = rand()%256;
        B = rand()%256;
     }
     else
     {
        color = color_set(z0,z1,z2 );
        R = color;
        G = color;
        B = color;
     }

     C[0].x = round(x0);
     C[0].y = round(y0);
     C[0].z = round(z0);    
     C[1].x = round(x1);
     C[1].y = round(y1);
     C[1].z = round(z1);    
     C[2].x = round(x2);
     C[2].y = round(y2);
     C[2].z = round(z2);
     
     // sort C with y  bubber sort
     for( i = 0 ; i < 3 ; i++ )
     {
        for( j = 0 ; j < 3 - 1 ; j++ )
        {
             if ( C[j].y > C[j+1].y )
                swap_C( &C[j] , &C[j+1] );    
        }
     }
     // count m
     Del_x01 = C[0].x - C[1].x;
     Del_y01 = C[0].y - C[1].y;     
     Del_x02 = C[0].x - C[2].x;
     Del_y02 = C[0].y - C[2].y; 
     Del_x12 = C[1].x - C[2].x;
     Del_y12 = C[1].y - C[2].y; 
     // m01
     if (Del_y01 != 0)
     {
          m01 = ( Del_x01 ) / ( Del_y01 );
          C01 = C[0].x - m01 * C[0].y;
     }
     else if ( Del_x01 == 0 )  // Del_x01 == 0 && Del_y01 == 0
     {
          m01 = 1;
          C01 = C[0].x - C[0].y;
     }
     else
     {
         //
     }
     // m02
     if (Del_y02 != 0)
     {
          m02 = ( Del_x02 ) / ( Del_y02 );
          C02 = C[0].x - m02 * C[0].y;
     }
     else if ( Del_x02 == 0 )  // 
     {
          m02 = 1;
          C02 = C[0].x - C[0].y;
     }
     else
     {
          //
     }
     // m12
     if (Del_y12 != 0)
     {
          m12 = ( Del_x12 ) / ( Del_y12 );
     }
     else if ( Del_x12 == 0 )
     {
          m12 = 1;
     }
     else
     {
          //
     }
     C01 = C[0].x;
     C02 = C[0].x;
     
     for ( i = C[0].y ; i < C[1].y ; i++ )   // draw C[0].y to C[1].y
     {
          
          draw_scan_Y_line(C01,C02,i,R,G,B);
          C01 += m01;
          C02 += m02;
     }
     C12 = C[1].x;

     for ( i = C[1].y ; i < C[2].y ; i++ )
     {
          //if (C12 > 600)
             //system("pause");
          draw_scan_Y_line(C12,C02,i,R,G,B);
          C12 += m12;
          C02 += m02;
     }
       
}


void swap_C( Vertex *var1 , Vertex *var2 )
{
     double tmp;
     tmp     = var1->x;
     var1->x = var2->x;
     var2->x = tmp;
     tmp     = var1->y;
     var1->y = var2->y;
     var2->y = tmp;
     tmp     = var1->z;
     var1->z = var2->z;
     var2->z = tmp;
}

void draw_pixel( int x , int y , int R , int G ,int B )
{
     screen_buffer[y][x].R = R;
     screen_buffer[y][x].G = G;
     screen_buffer[y][x].B = B;
}

void draw_scan_Y_line(int x0 , int x1 , int y , int R , int G ,int B )
{
     int x;
     int tmp;
     
     if (x0 > x1) // if x0 is large than x1 , then swap x0 , x1
     {
         tmp = x1;
         x1  = x0;
         x0  = tmp;      
     }
     //scan line
     for ( x = x0 ; x < x1 ; x++ )
     {
         screen_buffer[y][x].R = R;
         screen_buffer[y][x].G = G;
         screen_buffer[y][x].B = B;
     }
}

void Output_ppm()
{
    FILE *out ;
    int i,j;
    out = fopen("output.ppm","w");
    srand(time(NULL));
    fprintf( out , "P3\n" );
    fprintf( out , "800 600\n" );
    fprintf( out , "255\n" );
    for(j=0;j<600;j++)
    {
         for(i=0;i<800;i++)
         {
              fprintf(out,"%d %d %d ",screen_buffer[j][i].R,screen_buffer[j][i].G,screen_buffer[j][i].B);
         }
    }
    fclose(out); 
}

void init_screenbuffer()
{
    int i , j;
    for(j=0;j<600;j++)
    {
         for(i=0;i<800;i++)
         {
              screen_buffer[j][i].R = 0;
              screen_buffer[j][i].G = 0;
              screen_buffer[j][i].B = 0;
         }
    }     
}

void insert_vertex_point(double x , double y , double z)
{
    Vertex *Ptr_creat;
    Vertex *Ptr_tmp;   
    
    Ptr_creat = (Vertex*)malloc(sizeof(Vertex));
    Ptr_creat->x = x;
    Ptr_creat->y = y;
    Ptr_creat->z = z;
    Ptr_creat->next = NULL;
    if( start_vertex_point == NULL )
    {
         start_vertex_point = Ptr_creat;
         end_vertex_point = Ptr_creat;
    }
    else
    {
         Ptr_tmp = end_vertex_point;
         Ptr_tmp->next = Ptr_creat;
         end_vertex_point = Ptr_creat;
    }
}

void insert_trangle(Vertex *node0 , Vertex *node1 , Vertex *node2 )
{
    Trangle *Ptr_creat;
    Trangle *Ptr_tmp;
    Ptr_creat = (Trangle*)malloc(sizeof(Trangle));
    Ptr_creat->node0 = node0;
    Ptr_creat->max_z = Ptr_creat->node0->z;
    Ptr_creat->node1 = node1;
    if ( Ptr_creat->node1->z > Ptr_creat->max_z )
         Ptr_creat->max_z = Ptr_creat->node1->z;
    Ptr_creat->node2 = node2;
    if ( Ptr_creat->node2->z > Ptr_creat->max_z )
         Ptr_creat->max_z = Ptr_creat->node2->z;
    
    if( start_node == NULL )
    {
         start_node = Ptr_creat;
         end_node = Ptr_creat;
    }
    else
    {
         Ptr_tmp = end_node;
         Ptr_tmp->next = Ptr_creat;
         end_node = Ptr_creat;
    }
    
}

void free_Vertex()
{
     Vertex *Ptr_tmp;
     Ptr_tmp = start_vertex_point;
     while( start_vertex_point != NULL)
     {
        start_vertex_point =  start_vertex_point->next;
        free(Ptr_tmp);
     }
}

void free_Trangle()
{
     Trangle *Ptr_tmp;
     Ptr_tmp = start_node;
     while( start_node != NULL)
     {
        start_node =  start_node->next;
        free(Ptr_tmp);
     }     
}
int read_file_and_draw()
{
    Vertex *N0 , *N1 , *N2;
    double x,y,z;
    double max_x,max_y,max_z;
    double min_x,min_y,min_z;
    FILE *input;
    char title[50];
    int vertex_number;
    int draw_times;
    int i;
    int point_count;
    int node0,node1,node2;
    int start_node;
    max_x = 0;
    max_y = 0;
    max_z = 0;
    min_x = 0;
    min_y = 0;
    min_z = 0;
    
    input = fopen ("data.txt","r");
    fscanf(input,"%s",title);      // read Vertices
    if( stricmp(title,"Vertices") != 0 ) //the first line is not Vertices
    {
         fprintf(stderr,"Input file syntax error!\n"); 
         return -1;  
    }
    fscanf(input,"%d",&vertex_number);
    for( i = 0 ; i < vertex_number ; i++ )
    {
         fscanf(input,"%lf%lf%lf",&x,&y,&z);
         insert_vertex_point(x,y,-z);
         
         if ( x > max_x )
            max_x = x;     
         else if ( x < min_x )
            min_x = x;
         if ( y > max_y )
            max_y = y;
         else if ( y < min_y )
            min_y = y;
         if ( z > max_z )
            max_z = z;
         else if ( z < min_z )
            min_z = z;
    }
    //Normalization
    Normalization( max_x , max_y , min_x , min_y , max_z , min_z);
    
    while( fscanf(input,"%s",title)  == 1 )
    {
         if( strcmp(title,"Line_segment") == 0)
         {
            fscanf( input , "%d" ,&draw_times );
            for( i = 0 ; i < draw_times ; i++ )
            {
                fscanf(input,"%d",&node0);
                fscanf(input,"%d",&node1);
                N0 = search_vertex(node0);
                N1 = search_vertex(node1);
                draw_line( N0->x , N0->y , N0->z , N1->x , N1->y , N1->z );
            }                              
         }
         
         else if( stricmp(title,"Line_strip") == 0 )
         {
            fscanf( input , "%d" ,&draw_times );
            fscanf(input,"%d",&node0);
            for( i = 0 ; i < draw_times ; i++ )
            {
                fscanf(input,"%d",&node1);
                N0 = search_vertex(node0);
                N1 = search_vertex(node1);
                draw_line( N0->x , N0->y , N0->z , N1->x , N1->y , N1->z );
                node0 = node1;
            }               
         }
         
         else if( stricmp(title,"Line_loop") == 0 )
         {
            fscanf( input , "%d" ,&draw_times );
            fscanf(input,"%d",&node0);
            start_node = node0;
            for( i = 0 ; i < draw_times -1; i++ )
            {
               fscanf(input,"%d",&node1);
               N0 = search_vertex(node0);
               N1 = search_vertex(node1);
               draw_line( N0->x , N0->y , N0->z , N1->x , N1->y , N1->z );
               node0 = node1;
            }
            N1 = search_vertex(start_node);
            N0 = search_vertex(node0);
            draw_line( N0->x , N0->y , N0->z , N1->x , N1->y , N1->z );
         }
         
         else if( stricmp(title,"Triangle_fan") == 0)
         {
            fscanf( input , "%d" ,&draw_times );
            fscanf( input , "%d" , &node0 );
            fscanf( input , "%d" , &node1 );  
            N0 = search_vertex(node0);     // point A
            N1 = search_vertex(node1);     // point B
            start_node = node1; 
            for( i = 0 ; i < draw_times ; i++ )
            {
               fscanf( input , "%d" , &node2 ); 
               N2 = search_vertex(node2); // point C
               draw_triangle(N0->x,N0->y,N0->z,N1->x,N1->y,N1->z,N2->x,N2->y,N2->z);
               N1 = N2;
            }
              
         }
         
         else if (stricmp(title,"Triangle_list") == 0 )
         {
            fscanf( input , "%d" ,&draw_times );
            for( i = 0 ; i < draw_times ; i++ )
            {
               fscanf( input , "%d" , &node0 );
               fscanf( input , "%d" , &node1 ); 
               fscanf( input , "%d" , &node2 );
               N0 = search_vertex(node0);     // point A
               N1 = search_vertex(node1);     // point B
               N2 = search_vertex(node2);     // point C
               insert_trangle(N0,N1,N2);
               draw_triangle(N0->x,N0->y,N0->z,N1->x,N1->y,N1->z,N2->x,N2->y,N2->z);
            }
            //sort_trangle_node(draw_times)
            SORT = 1;
            trangle_number =  draw_times;             
         }
         
         else if( stricmp(title,"Triangle_strip") == 0 )
         {
            fscanf( input , "%d" ,&draw_times );
            fscanf( input , "%d" , &node0 );
            fscanf( input , "%d" , &node1 ); 
            N0 = search_vertex(node0);     // point A
            N1 = search_vertex(node1);     // point B
            for( i = 0 ; i < draw_times ; i++ )
            {
               fscanf( input , "%d" , &node2 );
               N2 = search_vertex(node2);     // point C
               draw_triangle(N0->x,N0->y,N0->z,N1->x,N1->y,N1->z,N2->x,N2->y,N2->z);
               N0 = N1;
               N1 = N2;  
            }
              
         }
    }
    //printf("%lf ,%lf , %lf\n",end_vertex_point->x,end_vertex_point->y,end_vertex_point->z);
    //system("pause");
}

void Normalization(double max_x , double max_y , double min_x , double min_y , double max_z , double min_z)
{
    Vertex *point_index;
    double x_move , y_move ,z_move;
    double scaling = 1;
    int i;
    
    x_move = ( max_x + min_x )/2;
    y_move = ( max_y + min_y )/2;
    z_move = ( max_z + min_y )/2;
    
    max_x -= x_move;
    min_x -= x_move;
    max_y -= y_move;
    min_y -= y_move;
    max_z -= z_move;
    min_z -= z_move;
    
    
    if ( max_x > 400 || max_y > 300 )           // out of screen
    {
         if( 400 - max_x > 300 - max_y )       // x is greater than y
             scaling = 400 / max_x;
         else                                  // y is greater than x
             scaling = 300 / max_y;
              
    }
    else if ( max_x < 200 && max_y < 150 )
    {
         do
         {
             scaling = scaling * 2;
         }while( max_x * scaling < 200 && max_y * scaling < 150 );
    }
    
    point_index = start_vertex_point;
    while( point_index != NULL )
    {
         point_index->x = ( (point_index->x - x_move)*scaling ) + 399;
         point_index->y = 299 - ( (point_index->y - y_move)*scaling );
         point_index->z = ( point_index->z + z_move ) * scaling;
         point_index = point_index->next;
    }
}

Vertex *search_vertex( int index )         // linear O(n)
{
    Vertex *Ptr_index;
    int counter;
    Ptr_index = start_vertex_point;
    for( counter = 0 ; Ptr_index != NULL ; counter++  )
    {
         if( counter == index )
             return Ptr_index;
         Ptr_index = Ptr_index->next;
    }
       
}

Trangle *search_min_z()
{
    Trangle *Ptr_tmp;
    Trangle *min;
    //double z;
    Ptr_tmp = start_node;
    while(Ptr_tmp->max_z == -9999)
    {
         Ptr_tmp = Ptr_tmp->next;
    }
         min = Ptr_tmp;
    while(Ptr_tmp!=NULL)
    {
          if (Ptr_tmp->max_z > min->max_z && min->max_z != -9999)
              min = Ptr_tmp;
          Ptr_tmp = Ptr_tmp->next;               
    }
    min->max_z = -9999;
    
    return min;

}




void draw_sort_trangle( char *filename )
{
    FILE *out ;
    Trangle *Ptr_tmp;
    int i,j;
    out = fopen( filename ,"w");
    Ptr_tmp = start_node;
    for (i =0 ; i < trangle_number; i++ )
    {
        Ptr_tmp = search_min_z();
        draw_triangle(Ptr_tmp->node0->x,Ptr_tmp->node0->y,Ptr_tmp->node0->z,
                      Ptr_tmp->node1->x,Ptr_tmp->node1->y,Ptr_tmp->node1->z,
                      Ptr_tmp->node2->x,Ptr_tmp->node2->y,Ptr_tmp->node2->z); 
                      
        //Ptr_tmp = Ptr_tmp->next;    
    }
    
    
    fprintf( out , "P3\n" );
    fprintf( out , "800 600\n" );
    fprintf( out , "255\n" );
    for(j=0;j<600;j++)
    {
         for(i=0;i<800;i++)
         {
              fprintf(out,"%d %d %d ",screen_buffer[j][i].R,screen_buffer[j][i].G,screen_buffer[j][i].B);
         }
    }
    fclose(out);      
}

int color_set( double z0 ,double z1 , double z2 )
{
    int color;
    color = ( 100 -( z0 + z1 + z2 )/3 )/2 + 80;
    return color; 
}


