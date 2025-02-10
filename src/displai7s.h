
// Mostra segment
void segment(int cr, int cg, int cb)
{
  for (int i = 0; i<num_led; i++)
  {
    pixels.setPixelColor(cnt_led, pixels.Color(cr,cg,cb));
    cnt_led++;
  }
}

// Mostra dígit
void digit(int dig, int cr, int cg, int cb)
{
    for (int i = 0; i<num_seg; i++)
  {
    int bi = (dseg[dig] >> i) & 1;
    if (bi != 0) segment(cr,cg,cb); else segment(0,0,0);
  }
}

// Mostra dades display
void dis_hormin()
{
    cnt_led = 0;
    int verm = 0;
    int ver = 0;
    int bla = 0;
    if (dcol == 0){verm = vermell; ver = verd; bla = blau;}
    else if (dcol == 1){verm =  60; ver =   0; bla = 0;}
    else if (dcol == 2){verm = 120; ver =  30; bla = 0;}
    else if (dcol == 3){verm =   0; ver = 100; bla = 0;}

    
    for (int i = 0; i<num_dis; i++)
  {
    digit(dig_rell[i],verm,ver,bla);
  } 
}


// Mostra dades display
void dis_hormin1()
{
    cnt_led = 0;
    for (int i = 0; i<num_dis; i++)
  {
    digit(dig_rell[i],vermell,verd,blau);
  } 
}

