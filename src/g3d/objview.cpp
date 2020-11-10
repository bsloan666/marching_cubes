//		Blake G. Sloan
// 		a toy 3d Vizualizer c. 2004
#define APPNAME "pt_viz"


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <set>
#include <GL/gl.h>
#include <GL/glut.h>
#include <string.h>
//#include <stk/stkpublic.h>
char ** gargv;
int gargc;
int wireframe = 0;
int normals = 0;
int gstart =1;
int decimate, rez;
float ht_scale = 1.0;
int all = 0;
int textRGB = 0;
int scope = 0;
int surf = 0;
int tex = 0;
int cinbox = 0;
int mesh3d = 0;
int rgb = 0;
int red = 1;
int green = 1;
int blue = 1;
int color_plot_mode = 0;
int dot3d = 0;
enum
{
  spots,
  dots,
  blips,
  zots,
  flarts
};
int format = 0;
GLuint texID;
int pow_2;
float rdiv = 0.05f/RAND_MAX;
float transp = 1.0f;
float smokeval = 1.0f;
float x_w;
float z_w;
float vox_width = 2.0;
float deg2rad = 22.0/1260.0;
float pie = 3.14159;
float two_pie = pie * 2.0;
float half_pie = pie / 2.0;
float qtr_pie = pie / 4.0;
//float pi_slice = two_pie / 32.00; 
//float pi_slice = two_pie / 12.00; 
float pi_slice = two_pie / 8.00; 
int lastx,lasty;
float amount  = -300;
float scale   = 1;
float degrees = 45;

int oldx, oldy;
int action = 0;
int xtrans = 0;
float xrot = 0;
float yrot = 0;
int frame = -1;

void smooth_depth();
void smooth_all();
void center_all();

enum {
	NOTHING,
	DRAG_ROTATE,
	DRAG_TRANSLATE,
	DRAG_TIME
};

GLfloat diffuseMaterial[] = { 0.5,0.5,0.5,1.0};

void display();

class p3d 
{
  public:
  p3d(float a, float b, float c):x(a),y(b),z(c){} 
  float x;
  float y;
  float z;
};

class t2d 
{
  public:
  t2d(float a, float b):x(a),y(b){} 
  float x;
  float y;
};
class f3p 
{
  public:
  f3p(int a, int b, int c, int d, int e, int f):p1(a),p2(b), p3(c), n1(d), n2(e), n3(f) {} 
  int p1;
  int p2;
  int p3;

  int n1;
  int n2;
  int n3;
};
class f4p 
{
  public:
  f4p(int a, int b, int c, int d):p1(a),p2(b), p3(c), p4(d){} 
  int p1;
  int p2;
  int p3;
  int p4;
};
	std::vector<p3d> pts;
	std::vector<p3d> nrms;
	std::vector<t2d> txs;
	std::vector<f3p> fcs;
	std::vector<int> cix;
	std::vector<int> pt2nrm;
	std::vector<float> vals;

// the following two glut callbacks are required...
void motion( int x, int y){
	if(action == DRAG_ROTATE){
		degrees += (x - oldx)/10.f;
//		if(!spots)
			xrot += (y - oldy)/10.f;
//		else
//			xrot = 0.0;
	} else if(action == DRAG_TRANSLATE){
		amount -= (y - oldy);
	}
	glutPostRedisplay();
	oldx = x;
	oldy = y;
}
void mouse(int btn, int st, int x, int y){
	if(st==GLUT_DOWN){
		if(btn == GLUT_LEFT_BUTTON){
			action = DRAG_ROTATE;
		} else if(btn == GLUT_RIGHT_BUTTON){
			action = DRAG_TIME;
		} else if(btn == GLUT_MIDDLE_BUTTON){
			action = DRAG_TRANSLATE;
		}
	} else {
		action = NOTHING;	
	}
	glutPostRedisplay();
	oldx = x;
	oldy = y;
}

// draw a fake ground and sky plane
void sky(){
	glBegin(GL_QUADS);
	glColor4f(0.3,0.3,0.5,1.0);
	glVertex3f(-500,200,-200);
	glVertex3f(500,200,-200);
	glColor4f(0.8,0.8,0.8,1.0);
	glVertex3f(500,-5, -200);
	glVertex3f(-500,-5, -200);
	glVertex3f(-500,-5,-200);
	glVertex3f(500,-5,-200);
	glColor4f(0.2,0.3,0.2,1.0);
	glVertex3f(500,-200, -200);
	glVertex3f(-500,-200, -200);
	glEnd();
}
void idle(){
	glutPostRedisplay();
}
//	glut callback for hot-keys
void keyboard( unsigned char k, int x, int y )
{
    if (k == 27 || k == 'q') 
    {
        exit(0);
    }
		if(k == '.')frame++;
		if(k == ',')frame--;
		if(k == 'w')wireframe = wireframe==1?0:1;
		if(k == 'n')normals = normals==1?0:1;

		
	glutPostRedisplay();
}
//	glut callback for window resize events 
void reshape(int w, int h)
{
    if (h == 0) h = 1;
    
    glViewport(0, 0, w, h  );
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, (float)w/h, 0.1, 5000.0);
    glMatrixMode(GL_MODELVIEW);
		glShadeModel(GL_SMOOTH);
		glutPostRedisplay();
}



// 	view dependent splat function: draw a smooth
// 	airbrush spot facing the viewer...

void spot(int x, int y, int z,  float r, float g, float b){
	float f;
	float dense_color[4]={r,g,b, 0.15f};
	float clear_color[4]={r,g,b,0.0f};
	 glPushMatrix ();
	 glRotatef (xrot, 1, 0, 0);
	 glRotatef (degrees, 0, 1, 0);
	 glTranslatef (x-128, y-128, z-128);
	 glRotatef (-degrees, 0, 1, 0);
	 glRotatef (-xrot, 1, 0, 0);
	glBegin(GL_TRIANGLE_FAN);
	glColor4fv(dense_color);


	glVertex3f(0,0,0 );
// swap the line below for the line above for nice looking screen saver!
//	glVertex3f(x,y,z );
	glColor4fv(clear_color);
	for( f = 0.0;f < two_pie;f+=pi_slice ){
		glVertex3f(  	 sin(f) * vox_width, 
				 cos(f) * vox_width, 
				 0 );
	}
  glVertex3f (0, vox_width, 0.0);
	glEnd();
	glPopMatrix ();
}

void dot(int x, int y, int z,  float r, float g, float b){
	float f;
	float dense_color[4]={r,g,b, 1.00f};
	float clear_color[4]={r,g,b,0.0f};
	 glPushMatrix ();
	 glRotatef (xrot, 1, 0, 0);
	 glRotatef (degrees, 0, 1, 0);
	 glTranslatef (x-128, y-128, z-128);
	 glRotatef (-degrees, 0, 1, 0);
	 glRotatef (-xrot, 1, 0, 0);
	glBegin(GL_TRIANGLE_FAN);
	glColor4fv(dense_color);
	glVertex3f(0,0,0 );
	glColor4fv(clear_color);
	for( f = 0.0;f < two_pie;f+=pi_slice ){
		glVertex3f(  	 sin(f) *vox_width, 
				 cos(f) *vox_width, 
				 0 );
	}
  glVertex3f (0, vox_width, 0.0);
	glEnd();
	glPopMatrix ();
}
void blip(int x, int y, int z,  float r, float g, float b){
	float f;
	float dense_color[4]={r,g,b, 1.00f};
	float clear_color[4]={r,g,b,0.0f};
	 glPushMatrix ();
	 glRotatef (xrot, 1, 0, 0);
	 glRotatef (degrees, 0, 1, 0);
	 glTranslatef (x-128, y-128, z-128);
	 glRotatef (-degrees, 0, 1, 0);
	 glRotatef (-xrot, 1, 0, 0);
	glBegin(GL_POINTS);
	glColor4fv(dense_color);
	glVertex3f(0,0,0 );
	glEnd();
	glPopMatrix ();
}
void flart(int x, int y, int z,  int dx, int dy, int dz, float r, float g, float b){
	float f;
	float dense_color[4]={r,g,b, 1.00f};
	float clear_color[4]={r,g,b,0.0f};
	 glPushMatrix ();
	 glRotatef (xrot, 1, 0, 0);
	 glRotatef (degrees, 0, 1, 0);
	 glTranslatef (x-128, y-128, z-128);
	 glRotatef (-degrees, 0, 1, 0);
	 glRotatef (-xrot, 1, 0, 0);
	glBegin(GL_TRIANGLE_FAN);
	glColor4fv(dense_color);
	glVertex3f(0,0,0 );
	glColor4fv(clear_color);
	for( f = 0.0;f < two_pie;f+=pi_slice ){
		glVertex3f(  	 sin(f) *vox_width, 
				 cos(f) *vox_width, 
				 0 );
	}
	glVertex3f (0, vox_width, 0.0);
	glEnd();
	glPopMatrix ();
	glPushMatrix ();
	 glRotatef (xrot, 1, 0, 0);
	 glRotatef (degrees, 0, 1, 0);
	glBegin(GL_LINES);
	glColor4fv(dense_color);
	glVertex3f(x-128,y-128,z-128 );
	glColor4fv(clear_color);
	glVertex3f((x-128)-dx,(y-128)- dy,(z-128)- dz );
	glEnd();
	glPopMatrix ();
}
void zot(int x, int y, int z,  float r, float g, float b){
	float f;
	float dense_color[4]={r,g,b, 1.00f};
	float clear_color[4]={r,g,b,0.0f};
	 glPushMatrix ();
	 glRotatef (xrot, 1, 0, 0);
	 glRotatef (degrees, 0, 1, 0);
	 glTranslatef (x-128, y-128, z-128);
	 glRotatef (-degrees, 0, 1, 0);
	 glRotatef (-xrot, 1, 0, 0);
	glBegin(GL_TRIANGLE_FAN);
	glColor4fv(dense_color);
	glVertex3f(0,0,0 );
	glColor4fv(clear_color);
	for( f = 0.0;f < two_pie;f+=pi_slice ){
		glVertex3f(  	 sin(f) *vox_width, 
				 cos(f) *vox_width, 
				 0 );
	}
  glVertex3f (0, vox_width, 0.0);
	glEnd();
	glPopMatrix ();
}
void rgb_box(){
	glBegin(GL_LINE_LOOP);
	glColor4f(0.0,0.0,0.0, 1.0);
	glVertex3f(0,0,0);	
	glColor4f(0.0,1.0,0.0, 1.0);
	glVertex3f(0,255,0);	
	glColor4f(1.0,1.0,0.0, 1.0);
	glVertex3f(255,255,0);	
	glColor4f(1.0,0.0,0.0, 1.0);
	glVertex3f(255,0,0);	
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor4f(0.0,0.0,1.0, 1.0);
	glVertex3f(0,0,255);	
	glColor4f(0.0,1.0,1.0, 1.0);
	glVertex3f(0,255,255);	
	glColor4f(1.0,1.0,1.0, 1.0);
	glVertex3f(255,255,255);	
	glColor4f(1.0,0.0,1.0, 1.0);
	glVertex3f(255,0,255);	
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor4f(0.0,0.0,0.0, 1.0);
	glVertex3f(0,0,0);	
	glColor4f(0.0,0.0,1.0, 1.0);
	glVertex3f(0,0,255);	
	glColor4f(0.0,1.0,1.0, 1.0);
	glVertex3f(0,255,255);	
	glColor4f(0.0,1.0,0.0, 1.0);
	glVertex3f(0,255, 0);	
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor4f(1.0,0.0,0.0, 1.0);
	glVertex3f(255,0,0);	
	glColor4f(1.0,0.0,1.0, 1.0);
	glVertex3f(255,0,255);	
	glColor4f(1.0,1.0,1.0, 1.0);
	glVertex3f(255,255,255);	
	glColor4f(1.0,1.0,0.0, 1.0);
	glVertex3f(255,255, 0);	
	glEnd();
}
void cineon_box(){
	glBegin(GL_LINE_LOOP);
	glColor3f(24.f/255.f,24.f/255.f,24.f/255.f);	glVertex3f(24,24,24);	
	glColor3f(24.f/255.f,171.f/255.f,24.f/255.f);	glVertex3f(24,171,24);	
	glColor3f(171.f/255.f,171.f/255.f,24.f/255.f);	glVertex3f(171,171,24);	
	glColor3f(171.f/255.f,24.f/255.f,24.f/255.f);	glVertex3f(171,24,24);	
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor3f(24.f/255.f,24.f/255.f,171.f/255.f);	glVertex3f(24,24,171);	
	glColor3f(24.f/255.f,171.f/255.f,171.f/255.f);	glVertex3f(24,171,171);	
	glColor3f(171.f/255.f,171.f/255.f,171.f/255.f);	glVertex3f(171,171,171);	
	glColor3f(171.f/255.f,24.f/255.f,171.f/255.f);	glVertex3f(171,24,171);	
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor3f(24.f/255.f,24.f/255.f,24.f/255.f);	glVertex3f(24,24,24);	
	glColor3f(24.f/255.f,24.f/255.f,171.f/255.f);	glVertex3f(24,24,171);	
	glColor3f(24.f/255.f,171.f/255.f,171.f/255.f);	glVertex3f(24,171,171);	
	glColor3f(24.f/255.f,171.f/255.f, 24.f/255.f);	glVertex3f(24,171, 24);	
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor3f(171.f/255.f,24.f/255.f,24.f/255.f);	glVertex3f(171,24,24);	
	glColor3f(171.f/255.f,24.f/255.f,171.f/255.f);	glVertex3f(171,24,171);	
	glColor3f(171.f/255.f,171.f/255.f,171.f/255.f);glVertex3f(171,171,171);	
	glColor3f(171.f/255.f,171.f/255.f, 24.f/255.f);glVertex3f(171,171, 24);	
	glEnd();

}





char fstring[1024];
void display()
{
	unsigned int i = 0;
    glClearColor(0.3f, 0.3f, 0.3f, 1.f);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0,0  ,amount);
	float inv = 1.0/255.0;
	glPushMatrix ();
	glRotatef (xrot, 1, 0, 0);
	glRotatef (degrees, 0, 1, 0);
	glTranslatef(-128,-128,-128);

	  rgb_box();


	  glPopMatrix ();
	  vox_width = 3;	
	  glPushMatrix ();
	  glTranslatef(0,0  ,amount);
	  glRotatef (xrot, 1, 0, 0);
	  glRotatef (degrees, 0, 1, 0);
//	  glTranslatef(128,128,128);
	  glColor4fv(diffuseMaterial);
	  int w =  800;
	  int h =  800;
	  float unity =  1;
	  glBegin(GL_TRIANGLES);	
	  for(i = 0;i<fcs.size();i++)
	  {
		int p1 = fcs[i].p1;
		int p2 = fcs[i].p2;
		int p3 = fcs[i].p3;

		int n1 = fcs[i].n1;
		int n2 = fcs[i].n2;
		int n3 = fcs[i].n3;
		glNormal3f( nrms[n1].x,  nrms[n1].y,  nrms[n1].z  );
		glVertex3f( pts[p1].x , pts[p1].y , pts[p1].z ) ;
		glNormal3f( nrms[n2].x,  nrms[n2].y,  nrms[n2].z  );
		glVertex3f( pts[p2].x , pts[p2].y , pts[p2].z );
		glNormal3f( nrms[n3].x,  nrms[n3].y,  nrms[n3].z  );
		glVertex3f( pts[p3].x , pts[p3].y , pts[p3].z );
	  }
	  glEnd();	
//	  glDisable(GL_TEXTURE_2D);
	  glPopMatrix ();
	if(wireframe)	
	{
	 glDisable(GL_LIGHTING);	
	  vox_width = 3;	
	  glPushMatrix ();
	  glTranslatef(0,0  ,amount);
	  glRotatef (xrot, 1, 0, 0);
	  glRotatef (degrees, 0, 1, 0);
//	  glTranslatef(128,128,128);
	  glColor4f(1.0,0.0,0.0,1.0);
	  glDisable(GL_BLEND);
	  for(i = 0;i<fcs.size();i++)
	  {
		  glBegin(GL_LINE_LOOP);	
		  int p1 = fcs[i].p1;
		  int p2 = fcs[i].p2;
		  int p3 = fcs[i].p3;
		  glVertex3f( pts[p1].x  , pts[p1].y , pts[p1].z ) ;
		  glVertex3f( pts[p2].x  , pts[p2].y , pts[p2].z  );
		  glVertex3f( pts[p3].x  , pts[p3].y , pts[p3].z );
		  glEnd();	
	  }
	  glPopMatrix ();
	 glEnable(GL_LIGHTING);	
	}
        	
	if(normals)	
	{
	 glDisable(GL_LIGHTING);	
	  vox_width = 3;	
	  glPushMatrix ();
	  glTranslatef(0,0  ,amount);
	  glRotatef (xrot, 1, 0, 0);
	  glRotatef (degrees, 0, 1, 0);
//	  glTranslatef(128,128,128);
	  glColor4f(0.0,1.0,0.0,1.0);
	  glDisable(GL_BLEND);
	  for(i = 0;i<fcs.size();i++)
	  {
		int p1 = fcs[i].p1;

		int n1 = fcs[i].n1;
		  glBegin(GL_LINES);	
		  glVertex3f( pts[p1].x  , pts[p1].y , pts[p1].z ) ;
		  glVertex3f( pts[p1].x + nrms[n1].x *16  , pts[p1].y + nrms[n1].y * 16, pts[p1].z+nrms[n1].z * 16 ) ;
		  glEnd();	
	  }
	  glPopMatrix ();
	 glEnable(GL_LIGHTING);	
	}	
	glutSwapBuffers();
	
}
void init_tex(){
}
void controls()
{

	fprintf(stderr,"Mouse:\n" );
	fprintf(stderr,"\tleft button................rotate\n" );
	fprintf(stderr,"\tmiddle button...........translate\n" );
	fprintf(stderr,"\tright button...........jog frames\n" );
	fprintf(stderr,"Keys:\n" );
	fprintf(stderr,"\ta....tex-mapped luma height field\n" );
	fprintf(stderr,"\tr, g, and b.......enable channels\n" );
	fprintf(stderr,"\tm..........toggle 3D oscilliscope\n" );
	fprintf(stderr,"\tc..............toggle 95-685 cube\n" );
	fprintf(stderr,"\tv..............toggle 0-1024 cube\n" );
	fprintf(stderr,"\tx..............toggle image plane\n" );
	fprintf(stderr,"\tn..........toggle rgb point cloud\n" );
	fprintf(stderr,"\td...............toggle large dots\n" );
	fprintf(stderr,"\tf...............toggle small dots\n" );
	fprintf(stderr,"\tm...toggle wireframe height field\n" );
	fprintf(stderr,"\tz.....toggle surface height field\n" );
	fprintf(stderr,"\tl...............smooth depth only\n" );
	fprintf(stderr,"\tL....................smooth x,y,z\n" );
	fprintf(stderr,"\tC...center object in view frustum\n" );
	fprintf(stderr,"\tM..........toggle wireframe model\n" );
	fprintf(stderr,"\t'='................increase depth\n" );
	fprintf(stderr,"\t'-'................decrease depth\n" );
}

void usage(char **argv)
{
	fprintf(stderr,"Usage:\n\t%s <filelist>\n\nExample:\n\t%s /images/foo_01*.cin\n\n", APPNAME, APPNAME);
  	fprintf(stderr,"Description:\n");
  	fprintf(stderr, "\tA 3D color space visualizer.\n");
	controls();
  	fprintf(stderr,"Notes:\n"); 
  	fprintf(stderr, "\tRefer to http://www.pactitle.com/pt/doc/pt_viz.html\n");
  	fprintf(stderr,"Bugs:\n"); 
  	fprintf(stderr, "\tReport bugs to bsloan@pactitle.com or use the automatic\n");
  	fprintf(stderr, "\tbug report engine at http://calypso/bsloan/cgi-bin/viewbugs.cgi\n");

}
#if(0)
void smooth_depth()
{
  unsigned int i, j, k;
  unsigned int npts = pts.size();
  unsigned int nfcs = fcs.size();

  std::vector <float> tmp;

  for(i = 0; i<npts; i++)
  {
	std::set <unsigned int> nbrs;
	for(j = 0;j<nfcs;j++)
	{
	  if( fcs[j].p1 == i)
	  {
		nbrs.insert(fcs[j].p2); 
		nbrs.insert(fcs[j].p3);
	  }
	  else if( fcs[j].p2 == i)
	  { 
		nbrs.insert(fcs[j].p1); 
		nbrs.insert(fcs[j].p3);
	  }	  
	  else if( fcs[j].p3 == i)
	  { 
		nbrs.insert(fcs[j].p1); 
		nbrs.insert(fcs[j].p2);
	  }	  
	}
	float accum = 0.f;	
	int wt = 0;	
	std::set <unsigned int>::iterator it = nbrs.begin();
	while(it != nbrs.end())
	{
	  accum += pts[(*it)].z;
	  wt++; 
	  it++;
	}
	tmp.push_back( accum/wt);
  }
  for(i = 0; i<npts; i++)
  {
	pts[i].z = tmp[i];
  }
}
#endif
void smooth_all()
{
  unsigned int i, j;
  unsigned int npts = pts.size();
  unsigned int nfcs = fcs.size();

  std::vector <p3d> tmp;
  std::vector <t2d> txp;

  for(i = 0; i<npts; i++)
  {
	std::set <unsigned int> nbrs;
	for(j = 0;j<nfcs;j++)
	{
	  if( (unsigned)fcs[j].p1 == i)
	  {
		nbrs.insert(fcs[j].p2); 
		nbrs.insert(fcs[j].p3);
	  }
	  else if( (unsigned)fcs[j].p2 == i)
	  { 
		nbrs.insert(fcs[j].p1); 
		nbrs.insert(fcs[j].p3);
	  }	  
	  else if( (unsigned)fcs[j].p3 == i)
	  { 
		nbrs.insert(fcs[j].p1); 
		nbrs.insert(fcs[j].p2);
	  }	  
	}
	p3d accum(0.f, 0.f, 0.f);	
	t2d taccum(0.f, 0.f);	
	int wt = 0;	
	std::set <unsigned int>::iterator it = nbrs.begin();
	while(it != nbrs.end())
	{
	  accum.x += pts[(*it)].x;
	  accum.y += pts[(*it)].y;
	  accum.z += pts[(*it)].z;
	  taccum.x += txs[(*it)].x;
	  taccum.y += txs[(*it)].y;
	  wt++; 
	  it++;
	}
	accum.x/=(float)wt;
	accum.y/=(float)wt;
	accum.z/=(float)wt;
	taccum.x/=(float)wt;
	taccum.y/=(float)wt;
	tmp.push_back( accum);
	txp.push_back( taccum);
  }
  for(i = 0; i<npts; i++)
  {
	pts[i].x = tmp[i].x;
	pts[i].y = tmp[i].y;
	pts[i].z = tmp[i].z;

	txs[i].x = txp[i].x;
	txs[i].y = txp[i].y;
  }
}

void smooth_depth()
{
  unsigned int i, j;
  unsigned int npts = pts.size();
  unsigned int nfcs = fcs.size();

  std::vector <float> tmp;

  for(i = 0; i<npts; i++)
  {
	std::set <unsigned int> nbrs;
	for(j = 0;j<nfcs;j++)
	{
	  if( (unsigned)fcs[j].p1 == i)
	  {
		nbrs.insert(fcs[j].p2); 
		nbrs.insert(fcs[j].p3);
	  }
	  else if( (unsigned)fcs[j].p2 == i)
	  { 
		nbrs.insert(fcs[j].p1); 
		nbrs.insert(fcs[j].p3);
	  }	  
	  else if( (unsigned)fcs[j].p3 == i)
	  { 
		nbrs.insert(fcs[j].p1); 
		nbrs.insert(fcs[j].p2);
	  }	  
	}
	std::set <unsigned int>::iterator it = nbrs.begin();
	std::set <float> depths;
	depths.insert( pts[i].z);
	while(it != nbrs.end())
	{
	  depths.insert( pts[(*it)].z);
	  it++;
	}
	std::set <float>::iterator median = depths.begin();
	for(j =0;j<depths.size()/2;j++)
	{
	  median++;
	}
	tmp.push_back( *median);
  }
  for(i = 0; i<npts; i++)
  {
	pts[i].z = tmp[i];
  }
}
void center_all()
{
  unsigned int i;
  unsigned int npts = pts.size();

  p3d min(20000.f, 20000.f, 20000.f);
  p3d max(-20000.f, -20000.f, -20000.f);


  for(i = 0; i<npts; i++)
  {
	if(pts[i].x < min.x)
	{
	  min.x = pts[i].x;
	}
	if(pts[i].y < min.y)
	{
	  min.y = pts[i].y;
	}
	if(pts[i].z < min.z)
	{
	  min.z = pts[i].z;
	}
	if(pts[i].x > max.x)
	{
	  max.x = pts[i].x;
	}
	if(pts[i].y > max.y)
	{
	  max.y = pts[i].y;
	}
	if(pts[i].z > max.z)
	{
	  max.z = pts[i].z;
	}
  }
  p3d mid((max.x - min.x)/2.f,  (max.y - min.y)/2.f,  (max.z - min.z)/2.f);  

fprintf(stderr, "mid = %4.4f, %4.4f, %4.4f\n", mid.x, mid.y, mid.z);

  p3d dif(min.x+mid.x, min.y+mid.y, min.z+mid.z);

  for(i = 0; i<npts; i++)
  {
	 pts[i].x -= dif.x; 
	 pts[i].y -= dif.y; 
	 pts[i].z -= dif.z; 
  }
}
void init()
{
	GLfloat mat_specular[] = { 1.0,1.0,1.0,1.0};
	GLfloat light_position[] = { 200.0,200.0,200.0,0.0};
	glClearColor(0.0,0.0,0.0,0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuseMaterial);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	glMaterialf(GL_FRONT,GL_SHININESS, 25.f);
	glLightfv(GL_LIGHT0,GL_POSITION, light_position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	 
}
int main(int argc, char ** argv){
	decimate = 1;
	rez = 256;
	gargc = argc;
	gargv = argv;
	if(argc <2 || (!(strcmp(argv[1], "-h")))) {
		usage(argv);
		exit(0);
	}
	controls();
	char *fname = argv[1];


	//pt_lock lock(APPNAME, "", argc, argv);
	if(!(strcmp("-rez", argv[1]))){
		rez = atoi(argv[2]);
		fname = argv[3];
		gstart = 3;
	} else if(!(strcmp("-3d", argv[1]))){

	  rez = 2048;
	  fprintf(stderr, "attempting to open file: %s...\n", argv[2]);
	  FILE *fp = fopen(argv[2], "r");
	  int pt = 1;
	  if(fp)
	  {

		float x, y, z;	
		int p1, p2, p3, p4, t1,t2,t3,t4,n1, n2, n3, n4, ix;	
		int p,t,n;	
		char b[8];
		char buff[1024];
		while (fscanf(fp, "%s",b) == 1)
		{ 
			if(! strcmp( "v", b))
			{
			  fscanf(fp, " %f %f %f\n",&x,&y,&z);
			  pts.push_back(p3d(x*256,y*256,z*256));
			  pt2nrm.push_back(pt++);
			  cix.push_back(ix);
			}
			else if(! strcmp( "g", b))
			{
			  fscanf(fp, " %d\n",&ix);
			}	
			else if(! strcmp( "vt", b))
			{
			  fscanf(fp, " %f %f\n",&x,&y);
			  txs.push_back(t2d(x,y));
			}	
			else if(! strcmp( "vn", b))
			{
			  fscanf(fp, " %f %f %f\n",&x,&y, &z);
			  nrms.push_back(p3d(x,y,z));
			}	
			else if(! strcmp( "f", b))
			{
			  	p4 = -1;
			  	n4 = -1;
			
				fgets(buff, 1024, fp);		 
				if(sscanf(buff, " %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
							&p1, &t1, &n1, &p2, &t2, &n2, 
							&p3, &t3, &n3, &p4, &t4, &n4) == 12)
				{
					fcs.push_back(f3p(p1-1,p2-1,p3-1, n1-1, n2- 1, n3- 1));
					fcs.push_back(f3p(p1-1,p3-1,p4-1, n1-1, n3-1, n4-1));
					pt2nrm[p1 - 1] = n1 - 1; 
					pt2nrm[p2 - 1] = n2 - 1; 
					pt2nrm[p3 - 1] = n3 - 1; 
					pt2nrm[p4 - 1] = n4 - 1; 
				}
				else
				{
					fcs.push_back(f3p(p1-1,p2-1,p3-1, n1-1, n2-1, n3-1));
					pt2nrm[p1 - 1] = n1 - 1; 
					pt2nrm[p2 - 1] = n2 - 1; 
					pt2nrm[p3 - 1] = n3 - 1; 
				}
			}
		} 
			
		fclose(fp);
		color_plot_mode = 999;
		fname = argv[3];
		gstart = 3;
		dot3d = 1;
	  	fprintf(stderr, "done reading  file: %s...\n", argv[2]);
	  }	
	}
		

// caution: command line parser expects ordered frames...
// initialize glut
	glutInit(&argc,argv);
    glutInitDisplayMode( GLUT_DOUBLE |  GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(10, 10);
    glutInitWindowSize(800,670 );
    glutCreateWindow("bViz");


	init();
// set glut callbacks


	idle();

	gluPerspective(90, 1.0, -1, -2000);
	glViewport(0,0,800, 800);
  
	if(!textRGB){
		glutIdleFunc( idle);
	}

	glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutReshapeFunc(reshape);
	glutDisplayFunc(display);

// begin event handling...
	glutMainLoop();
	return 1;
}		
