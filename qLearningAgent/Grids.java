/***************************************************************************/
/*Description : Modified original code so that the grid would display      */
/*room numbers of the 4th floor in WWU cs building and also a path that    */
/*is specified in the input files this program uses.                       */
/*Author of modification : David Nachmanson                                */
/*Last Date modified : December 13, 2017                                   */
/***************************************************************************/                        

/*
 * Copyright (c) Ian F. Darwin, http://www.darwinsys.com/, 1996-2002.
 * All rights reserved. Software written by Ian F. Darwin and others.
 * $Id: LICENSE,v 1.8 2004/02/09 03:33:38 ian Exp $
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Java, the Duke mascot, and all variants of Sun's Java "steaming coffee
 * cup" logo are trademarks of Sun Microsystems. Sun's, and James Gosling's,
 * pioneering role in inventing and promulgating (and standardizing) the Java
 * language and environment is gratefully acknowledged.
 *
 * The pioneering role of Dennis Ritchie and Bjarne Stroustrup, of AT&T, for
 * inventing predecessor languages C and C++ is also gratefully acknowledged.
 */

import java.awt.Canvas;
import java.io.File;
import java.util.Scanner;
import java.util.*;
import java.awt.Color;
import java.io.FileNotFoundException;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

/**
 * Program to draw grids.
 *
 * @author Ian Darwin, http://www.darwinsys.com/
 */
class GridsCanvas  extends Canvas   {
  int width, height;

  int rows;

  int cols;

  GridsCanvas(int w, int h, int r, int c) {
    setSize(width = w, height = h);
    rows = r;
    cols = c;
  }

  public void paint(Graphics g) {
    int i;
    width = getSize().width;
    height = getSize().height;
    String room;
    int roomLoc;
    File file = new File("roomToGridKey.txt");
    Scanner sc = null;
    Scanner userInput = new Scanner(System.in);
    try{
    sc = new Scanner(file);
  }catch(FileNotFoundException ex){
    System.exit(0);
  }

    // draw the rows
    int rowHt = height / (rows);
    for (i = 0; i < rows; i++)
      g.drawLine(0, i * rowHt, width, i * rowHt);

    // draw the columns
    int rowWid = width / (cols) ;
    for (i = 0; i < cols; i++)
      g.drawLine(i * rowWid, 0, i * rowWid, height);

    while(sc.hasNextLine()){
      room = sc.next();
      roomLoc = sc.nextInt();
      g.drawString(room,(roomLoc % 52) * rowWid + 5,((roomLoc/52) * rowHt) + rowHt/2);
    }

    int alpha = 110;//transparency
    Color lime = new Color(102,255,102,alpha);

    file = new File("path.txt");
    try{
      sc = new Scanner(file);
    }catch(FileNotFoundException ex){
       System.exit(0);
    }
    g.setColor(lime);
    while(sc.hasNext()){
      roomLoc = sc.nextInt();
      g.fillRect((roomLoc % 52) * rowWid , roomLoc/52 * rowHt,rowWid,rowHt);
      try {

			Thread.sleep(150);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
    }

    //g.drawString("489",6 * rowWid + 5,rowHt/2);
    //g.drawString("481",5 + 1 * rowWid,206/52 * rowHt + (rowHt/2) );

  }
}

/** This is the demo class. */

public class Grids extends Frame {
  /*
   * Construct a GfxDemo2 given its title, width and height. Uses a
   * GridBagLayout to make the Canvas resize properly.
   */
  Grids(String title, int w, int h, int rows, int cols) {
    setTitle(title);

    // Now create a Canvas and add it to the Frame.
    GridsCanvas xyz = new GridsCanvas(w, h, rows, cols);
    add(xyz);

    addWindowListener(new WindowAdapter() {
      public void windowClosing(WindowEvent e) {
        setVisible(false);
        dispose();
        System.exit(0);
      }
    });

    // Normal end ... pack it up!
    pack();
  }

  public static void main(String[] a) {
    new Grids("4th floor cs building", 1000, 700, 16, 52).setVisible(true);
  }
}
