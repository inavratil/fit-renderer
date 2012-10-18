/**
****************************************************************************************************
****************************************************************************************************
@file: font.cpp
@brief draws various text on screen. 2D is drawn using texture fonts
Code adapted from: http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=17
****************************************************************************************************
***************************************************************************************************/
#include "scene.h"
///Spacing between letters
#define FONT_SPACING 10

/**
****************************************************************************************************
@brief Create display lists for all characters(256) in font. Font is read from file.
****************************************************************************************************/
void TScene::BuildFont()
{
    float cx,cy;         //x and y coordinate in texture

    //load font and background texture
    m_font2D_bkg = new Texture();
    m_font2D_bkg->Load("font_bkg",BASE,"data/font_bkg.tga",MODULATE,1.0,1.0,1.0,false,false,-1);
    m_font2D_tex = new Texture();
    m_font2D_tex->Load("font",BASE,"data/font.tga",MODULATE,1.0,1.0,1.0,false,false,-1);

    m_font2D = glGenLists(256);     //256 display lists
    glBindTexture(GL_TEXTURE_2D, m_font2D_tex->GetID());     //choose texture
    //generate 256 characters from texture and make display lists from them
    for (int i = 0; i < 16; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            cx = j/16.0f;        //x position in texture
            cy = i/16.0f;        //y position in texture

            glNewList(m_font2D+i*16+j,GL_COMPILE);
            glBegin(GL_QUADS);            //1 char = 1 quad
            glTexCoord2f(cx,1.0f - cy - 0.0625f);            glVertex2i(0,0);
            glTexCoord2f(cx + 0.0625f, 1.0f -cy - 0.0625f);  glVertex2i(16,0);
            glTexCoord2f(cx + 0.0625f, 1.0f - cy);           glVertex2i(16,16);
            glTexCoord2f(cx, 1.0f - cy);                     glVertex2i(0,16);
            glEnd();

            glTranslated(FONT_SPACING,0,0);           //move to next character
            glEndList();
        }
    }
}


/**
****************************************************************************************************
@brief Print 2D texture font on screen
@param s text to be drawn
@param px text position in % relative to screen width
@param py text position in % relative to screen height
@param size font size (default 1.0)
@param center shoud text be centered? Be default, it is aligned to the left
****************************************************************************************************/
void TScene::DrawScreenText(const char *s, float px, float py, float size, bool center)
{
    if(px < 0 || py < 0)
        return;
    //calculate real text position
    float x = m_resx*(px/100.0f);
    float y = m_resy*(py/100.0f);
    float sizex = size*FONT_SPACING*strlen(s);
    float sizey = FONT_SPACING*size;

    float offsetx;
    if(center)
        offsetx = 2.0f*sizex/3.0f;
    else
        offsetx = sizex;

    ///turn off shaders before text drawing
    glUseProgram(0);

    //create orthogonal projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,m_resx,0,m_resy,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);                       //disable z-buffer

    //quad in background
    glColor4f(1.0,1.0,1.0,1.0);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, m_font2D_bkg->GetID()); //choose background texture
    glBegin(GL_QUADS);
    glTexCoord2f(0.0,0.0); glVertex2f(x - offsetx, y - sizey);
    glTexCoord2f(1.0,0.0); glVertex2f(x + offsetx, y - sizey);
    glTexCoord2f(1.0,1.0); glVertex2f(x + offsetx, y + 2*sizey);
    glTexCoord2f(0.0,1.0); glVertex2f(x - offsetx, y + 2*sizey);
    glEnd();

    //font drawing
    glBindTexture(GL_TEXTURE_2D, m_font2D_tex->GetID()); //choose font texture
    glColor4f(1.0,0.5,0.0,1.0);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);

    if(center)
        glTranslatef(x - sizex/2,y - sizey/2,0); //translate to center of drawing position
    else
        glTranslatef(x, y, 0);      //else to the first character

    glListBase(m_font2D);
    glScalef(size,size,size);
    glCallLists(strlen(s),GL_BYTE,s);

    glEnable(GL_DEPTH_TEST);                  //re-enable depth buffer
    glDisable(GL_BLEND);
}
