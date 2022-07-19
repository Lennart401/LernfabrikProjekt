#include <math.h>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <cstring>
#include "pffft.h"
#include "Row.h"

int main() {
    int nrows = 128;
    float sample[15];
    Row rows[nrows];
    PFFFT_Setup *my_setup = pffft_new_setup(nrows, PFFFT_REAL);

    rows[0].timestamp = 0; rows[0].acc_x = -1.815425; rows[0].acc_y = -1.159189; rows[0].acc_z = 13.095966; rows[0].acc_abs = 13.271919;
    rows[1].timestamp = 10; rows[1].acc_x = -1.815425; rows[1].acc_y = -1.159189; rows[1].acc_z = 13.095966; rows[1].acc_abs = 13.271919;
    rows[2].timestamp = 20; rows[2].acc_x = -1.877695; rows[2].acc_y = -1.116079; rows[2].acc_z = 13.368999; rows[2].acc_abs = 13.546273;
    rows[3].timestamp = 30; rows[3].acc_x = -1.887275; rows[3].acc_y = -1.053809; rows[3].acc_z = 13.479170; rows[3].acc_abs = 13.651386;
    rows[4].timestamp = 40; rows[4].acc_x = -1.882485; rows[4].acc_y = -1.025068; rows[4].acc_z = 13.503120; rows[4].acc_abs = 13.672190;
    rows[5].timestamp = 50; rows[5].acc_x = -1.872905; rows[5].acc_y = -0.977168; rows[5].acc_z = 13.493540; rows[5].acc_abs = 13.657901;
    rows[6].timestamp = 60; rows[6].acc_x = -1.877695; rows[6].acc_y = -0.871787; rows[6].acc_z = 13.436060; rows[6].acc_abs = 13.594611;
    rows[7].timestamp = 70; rows[7].acc_x = -1.916016; rows[7].acc_y = -0.718506; rows[7].acc_z = 13.383369; rows[7].acc_abs = 13.538905;
    rows[8].timestamp = 80; rows[8].acc_x = -1.959126; rows[8].acc_y = -0.512534; rows[8].acc_z = 13.287568; rows[8].acc_abs = 13.440994;
    rows[9].timestamp = 90; rows[9].acc_x = -1.997446; rows[9].acc_y = -0.268242; rows[9].acc_z = 13.158237; rows[9].acc_abs = 13.311685;
    rows[10].timestamp = 100; rows[10].acc_x = -2.030977; rows[10].acc_y = -0.019160; rows[10].acc_z = 13.004956; rows[10].acc_abs = 13.162603;
    rows[11].timestamp = 110; rows[11].acc_x = -2.050137; rows[11].acc_y = 0.205972; rows[11].acc_z = 12.803775; rows[11].acc_abs = 12.968506;
    rows[12].timestamp = 120; rows[12].acc_x = -2.040557; rows[12].acc_y = 0.383203; rows[12].acc_z = 12.549902; rows[12].acc_abs = 12.720486;
    rows[13].timestamp = 130; rows[13].acc_x = -2.011817; rows[13].acc_y = 0.488584; rows[13].acc_z = 12.286450; rows[13].acc_abs = 12.459654;
    rows[14].timestamp = 140; rows[14].acc_x = -1.959126; rows[14].acc_y = 0.560435; rows[14].acc_z = 12.008628; rows[14].acc_abs = 12.180288;
    rows[15].timestamp = 150; rows[15].acc_x = -1.887275; rows[15].acc_y = 0.608335; rows[15].acc_z = 11.697275; rows[15].acc_abs = 11.864153;
    rows[16].timestamp = 160; rows[16].acc_x = -1.796265; rows[16].acc_y = 0.608335; rows[16].acc_z = 11.347603; rows[16].acc_abs = 11.504987;
    rows[17].timestamp = 170; rows[17].acc_x = -1.700464; rows[17].acc_y = 0.555645; rows[17].acc_z = 10.930869; rows[17].acc_abs = 11.076291;
    rows[18].timestamp = 180; rows[18].acc_x = -1.590293; rows[18].acc_y = 0.474214; rows[18].acc_z = 10.437495; rows[18].acc_abs = 10.568596;
    rows[19].timestamp = 190; rows[19].acc_x = -1.504072; rows[19].acc_y = 0.392783; rows[19].acc_z = 9.924961; rows[19].acc_abs = 10.045963;
    rows[20].timestamp = 200; rows[20].acc_x = -1.470542; rows[20].acc_y = 0.354463; rows[20].acc_z = 9.412427; rows[20].acc_abs = 9.533201;
    rows[21].timestamp = 210; rows[21].acc_x = -1.494492; rows[21].acc_y = 0.373623; rows[21].acc_z = 8.938213; rows[21].acc_abs = 9.069992;
    rows[22].timestamp = 220; rows[22].acc_x = -1.561553; rows[22].acc_y = 0.455054; rows[22].acc_z = 8.502319; rows[22].acc_abs = 8.656498;
    rows[23].timestamp = 230; rows[23].acc_x = -1.647773; rows[23].acc_y = 0.550855; rows[23].acc_z = 8.109536; rows[23].acc_abs = 8.293562;
    rows[24].timestamp = 240; rows[24].acc_x = -1.738784; rows[24].acc_y = 0.665815; rows[24].acc_z = 7.812554; rows[24].acc_abs = 8.031356;
    rows[25].timestamp = 250; rows[25].acc_x = -1.839375; rows[25].acc_y = 0.809517; rows[25].acc_z = 7.587422; rows[25].acc_abs = 7.849050;
    rows[26].timestamp = 260; rows[26].acc_x = -1.959126; rows[26].acc_y = 0.929268; rows[26].acc_z = 7.357500; rows[26].acc_abs = 7.670366;
    rows[27].timestamp = 270; rows[27].acc_x = -2.083667; rows[27].acc_y = 1.015488; rows[27].acc_z = 7.094048; rows[27].acc_abs = 7.463136;
    rows[28].timestamp = 280; rows[28].acc_x = -2.198628; rows[28].acc_y = 1.077759; rows[28].acc_z = 6.821016; rows[28].acc_abs = 7.247192;
    rows[29].timestamp = 290; rows[29].acc_x = -2.318379; rows[29].acc_y = 1.116079; rows[29].acc_z = 6.543193; rows[29].acc_abs = 7.030924;
    rows[30].timestamp = 300; rows[30].acc_x = -2.418970; rows[30].acc_y = 1.135239; rows[30].acc_z = 6.274951; rows[30].acc_abs = 6.820205;
    rows[31].timestamp = 310; rows[31].acc_x = -2.495610; rows[31].acc_y = 1.144819; rows[31].acc_z = 6.006709; rows[31].acc_abs = 6.604486;
    rows[32].timestamp = 320; rows[32].acc_x = -2.538721; rows[32].acc_y = 1.168769; rows[32].acc_z = 5.810318; rows[32].acc_abs = 6.447551;
    rows[33].timestamp = 330; rows[33].acc_x = -2.567461; rows[33].acc_y = 1.211880; rows[33].acc_z = 5.685776; rows[33].acc_abs = 6.355199;
    rows[34].timestamp = 340; rows[34].acc_x = -2.600991; rows[34].acc_y = 1.274150; rows[34].acc_z = 5.661826; rows[34].acc_abs = 6.359630;
    rows[35].timestamp = 350; rows[35].acc_x = -2.639312; rows[35].acc_y = 1.350791; rows[35].acc_z = 5.724097; rows[35].acc_abs = 6.446385;
    rows[36].timestamp = 360; rows[36].acc_x = -2.701582; rows[36].acc_y = 1.417852; rows[36].acc_z = 5.776787; rows[36].acc_abs = 6.533002;
    rows[37].timestamp = 370; rows[37].acc_x = -2.778223; rows[37].acc_y = 1.451382; rows[37].acc_z = 5.834268; rows[37].acc_abs = 6.622968;
    rows[38].timestamp = 380; rows[38].acc_x = -2.811753; rows[38].acc_y = 1.403481; rows[38].acc_z = 5.877378; rows[38].acc_abs = 6.664780;
    rows[39].timestamp = 390; rows[39].acc_x = -2.821333; rows[39].acc_y = 1.259780; rows[39].acc_z = 5.930068; rows[39].acc_abs = 6.686754;
    rows[40].timestamp = 400; rows[40].acc_x = -2.802173; rows[40].acc_y = 1.068179; rows[40].acc_z = 6.045029; rows[40].acc_abs = 6.748004;
    rows[41].timestamp = 410; rows[41].acc_x = -2.759063; rows[41].acc_y = 0.876577; rows[41].acc_z = 6.255791; rows[41].acc_abs = 6.893166;
    rows[42].timestamp = 420; rows[42].acc_x = -2.715952; rows[42].acc_y = 0.752036; rows[42].acc_z = 6.581514; rows[42].acc_abs = 7.159489;
    rows[43].timestamp = 430; rows[43].acc_x = -2.715952; rows[43].acc_y = 0.737666; rows[43].acc_z = 6.998247; rows[43].acc_abs = 7.542944;
    rows[44].timestamp = 440; rows[44].acc_x = -2.744692; rows[44].acc_y = 0.838257; rows[44].acc_z = 7.539522; rows[44].acc_abs = 8.067242;
    rows[45].timestamp = 450; rows[45].acc_x = -2.749482; rows[45].acc_y = 1.015488; rows[45].acc_z = 8.195757; rows[45].acc_abs = 8.704097;
    rows[46].timestamp = 460; rows[46].acc_x = -2.692002; rows[46].acc_y = 1.216670; rows[46].acc_z = 8.962163; rows[46].acc_abs = 9.436500;
    rows[47].timestamp = 470; rows[47].acc_x = -2.577041; rows[47].acc_y = 1.350791; rows[47].acc_z = 9.699829; rows[47].acc_abs = 10.126819;
    rows[48].timestamp = 480; rows[48].acc_x = -2.414180; rows[48].acc_y = 1.403481; rows[48].acc_z = 10.360854; rows[48].acc_abs = 10.730579;
    rows[49].timestamp = 490; rows[49].acc_x = -2.189048; rows[49].acc_y = 1.408271; rows[49].acc_z = 10.916499; rows[49].acc_abs = 11.222527;
    rows[50].timestamp = 500; rows[50].acc_x = -1.963916; rows[50].acc_y = 1.379531; rows[50].acc_z = 11.376343; rows[50].acc_abs = 11.626747;
    rows[51].timestamp = 510; rows[51].acc_x = -1.757944; rows[51].acc_y = 1.379531; rows[51].acc_z = 11.783496; rows[51].acc_abs = 11.993509;
    rows[52].timestamp = 520; rows[52].acc_x = -1.628613; rows[52].acc_y = 1.427432; rows[52].acc_z = 12.157119; rows[52].acc_abs = 12.348501;
    rows[53].timestamp = 530; rows[53].acc_x = -1.561553; rows[53].acc_y = 1.537603; rows[53].acc_z = 12.511582; rows[53].acc_abs = 12.702061;
    rows[54].timestamp = 540; rows[54].acc_x = -1.499282; rows[54].acc_y = 1.657354; rows[54].acc_z = 12.880415; rows[54].acc_abs = 13.072863;
    rows[55].timestamp = 550; rows[55].acc_x = -1.403481; rows[55].acc_y = 1.714834; rows[55].acc_z = 13.201347; rows[55].acc_abs = 13.386037;
    rows[56].timestamp = 560; rows[56].acc_x = -1.264570; rows[56].acc_y = 1.681304; rows[56].acc_z = 13.541440; rows[56].acc_abs = 13.703887;
    rows[57].timestamp = 570; rows[57].acc_x = -1.082549; rows[57].acc_y = 1.619033; rows[57].acc_z = 13.915064; rows[57].acc_abs = 14.050700;
    rows[58].timestamp = 580; rows[58].acc_x = -0.866997; rows[58].acc_y = 1.523232; rows[58].acc_z = 14.221626; rows[58].acc_abs = 14.329221;
    rows[59].timestamp = 590; rows[59].acc_x = -0.737666; rows[59].acc_y = 1.398691; rows[59].acc_z = 14.437178; rows[59].acc_abs = 14.523518;
    rows[60].timestamp = 600; rows[60].acc_x = -0.665815; rows[60].acc_y = 1.298101; rows[60].acc_z = 14.552138; rows[60].acc_abs = 14.625085;
    rows[61].timestamp = 610; rows[61].acc_x = -0.565225; rows[61].acc_y = 1.235830; rows[61].acc_z = 14.518608; rows[61].acc_abs = 14.582069;
    rows[62].timestamp = 620; rows[62].acc_x = -0.507744; rows[62].acc_y = 1.250200; rows[62].acc_z = 14.312636; rows[62].acc_abs = 14.376104;
    rows[63].timestamp = 630; rows[63].acc_x = -0.459844; rows[63].acc_y = 1.274150; rows[63].acc_z = 13.953383; rows[63].acc_abs = 14.018981;
    rows[64].timestamp = 640; rows[64].acc_x = -0.455054; rows[64].acc_y = 1.274150; rows[64].acc_z = 13.416900; rows[64].acc_abs = 13.484945;
    rows[65].timestamp = 650; rows[65].acc_x = -0.474214; rows[65].acc_y = 1.254990; rows[65].acc_z = 12.803775; rows[65].acc_abs = 12.873870;
    rows[66].timestamp = 660; rows[66].acc_x = -0.483794; rows[66].acc_y = 1.202300; rows[66].acc_z = 12.166699; rows[66].acc_abs = 12.235528;
    rows[67].timestamp = 670; rows[67].acc_x = -0.498164; rows[67].acc_y = 1.116079; rows[67].acc_z = 11.558364; rows[67].acc_abs = 11.622804;
    rows[68].timestamp = 680; rows[68].acc_x = -0.464634; rows[68].acc_y = 0.996328; rows[68].acc_z = 10.935659; rows[68].acc_abs = 10.990778;
    rows[69].timestamp = 690; rows[69].acc_x = -0.349673; rows[69].acc_y = 0.809517; rows[69].acc_z = 10.298584; rows[69].acc_abs = 10.336267;
    rows[70].timestamp = 700; rows[70].acc_x = -0.167651; rows[70].acc_y = 0.560435; rows[70].acc_z = 9.671089; rows[70].acc_abs = 9.688765;
    rows[71].timestamp = 710; rows[71].acc_x = 0.014370; rows[71].acc_y = 0.258662; rows[71].acc_z = 9.024433; rows[71].acc_abs = 9.028151;
    rows[72].timestamp = 720; rows[72].acc_x = 0.148491; rows[72].acc_y = -0.043110; rows[72].acc_z = 8.377778; rows[72].acc_abs = 8.379205;
    rows[73].timestamp = 730; rows[73].acc_x = 0.301772; rows[73].acc_y = -0.292192; rows[73].acc_z = 7.836504; rows[73].acc_abs = 7.847754;
    rows[74].timestamp = 740; rows[74].acc_x = 0.483794; rows[74].acc_y = -0.469424; rows[74].acc_z = 7.453301; rows[74].acc_abs = 7.483723;
    rows[75].timestamp = 750; rows[75].acc_x = 0.651445; rows[75].acc_y = -0.608335; rows[75].acc_z = 7.151528; rows[75].acc_abs = 7.206859;
    rows[76].timestamp = 760; rows[76].acc_x = 0.819097; rows[76].acc_y = -0.752036; rows[76].acc_z = 6.830595; rows[76].acc_abs = 6.920514;
    rows[77].timestamp = 770; rows[77].acc_x = 0.819097; rows[77].acc_y = -0.871787; rows[77].acc_z = 6.509663; rows[77].acc_abs = 6.618659;
    rows[78].timestamp = 780; rows[78].acc_x = 0.843047; rows[78].acc_y = -0.819097; rows[78].acc_z = 6.447392; rows[78].acc_abs = 6.553664;
    rows[79].timestamp = 790; rows[79].acc_x = 0.862207; rows[79].acc_y = -0.708926; rows[79].acc_z = 6.686894; rows[79].acc_abs = 6.779420;
    rows[80].timestamp = 800; rows[80].acc_x = 0.958008; rows[80].acc_y = -0.704136; rows[80].acc_z = 7.094048; rows[80].acc_abs = 7.192990;
    rows[81].timestamp = 810; rows[81].acc_x = 1.173560; rows[81].acc_y = -0.550855; rows[81].acc_z = 7.544312; rows[81].acc_abs = 7.654889;
    rows[82].timestamp = 820; rows[82].acc_x = 1.446592; rows[82].acc_y = -0.488584; rows[82].acc_z = 7.989785; rows[82].acc_abs = 8.134372;
    rows[83].timestamp = 830; rows[83].acc_x = 1.757944; rows[83].acc_y = -0.450264; rows[83].acc_z = 8.406519; rows[83].acc_abs = 8.600155;
    rows[84].timestamp = 840; rows[84].acc_x = 1.987866; rows[84].acc_y = -0.387993; rows[84].acc_z = 8.789721; rows[84].acc_abs = 9.020053;
    rows[85].timestamp = 850; rows[85].acc_x = 2.093247; rows[85].acc_y = -0.273032; rows[85].acc_z = 9.134604; rows[85].acc_abs = 9.375352;
    rows[86].timestamp = 860; rows[86].acc_x = 2.054927; rows[86].acc_y = -0.124541; rows[86].acc_z = 9.402846; rows[86].acc_abs = 9.625578;
    rows[87].timestamp = 870; rows[87].acc_x = 1.954336; rows[87].acc_y = 0.009580; rows[87].acc_z = 9.575288; rows[87].acc_abs = 9.772700;
    rows[88].timestamp = 880; rows[88].acc_x = 1.805845; rows[88].acc_y = 0.129331; rows[88].acc_z = 9.695039; rows[88].acc_abs = 9.862636;
    rows[89].timestamp = 890; rows[89].acc_x = 1.662144; rows[89].acc_y = 0.205972; rows[89].acc_z = 9.857901; rows[89].acc_abs = 9.999167;
    rows[90].timestamp = 900; rows[90].acc_x = 1.566343; rows[90].acc_y = 0.373623; rows[90].acc_z = 10.140512; rows[90].acc_abs = 10.267571;
    rows[91].timestamp = 910; rows[91].acc_x = 1.542393; rows[91].acc_y = 0.459844; rows[91].acc_z = 10.360854; rows[91].acc_abs = 10.485119;
    rows[92].timestamp = 920; rows[92].acc_x = 1.623823; rows[92].acc_y = 0.450264; rows[92].acc_z = 10.576406; rows[92].acc_abs = 10.709805;
    rows[93].timestamp = 930; rows[93].acc_x = 1.528022; rows[93].acc_y = 0.330513; rows[93].acc_z = 10.859018; rows[93].acc_abs = 10.970979;
    rows[94].timestamp = 940; rows[94].acc_x = -0.086221; rows[94].acc_y = -0.325723; rows[94].acc_z = 10.926079; rows[94].acc_abs = 10.931273;
    rows[95].timestamp = 950; rows[95].acc_x = -1.652563; rows[95].acc_y = -1.676514; rows[95].acc_z = 10.145303; rows[95].acc_abs = 10.414837;
    rows[96].timestamp = 960; rows[96].acc_x = -2.241738; rows[96].acc_y = -1.887275; rows[96].acc_z = 9.268725; rows[96].acc_abs = 9.720929;
    rows[97].timestamp = 970; rows[97].acc_x = -1.791475; rows[97].acc_y = -1.207090; rows[97].acc_z = 10.006392; rows[97].acc_abs = 10.236910;
    rows[98].timestamp = 980; rows[98].acc_x = -1.120869; rows[98].acc_y = -0.536484; rows[98].acc_z = 10.571616; rows[98].acc_abs = 10.644399;
    rows[99].timestamp = 990; rows[99].acc_x = -0.100591; rows[99].acc_y = 0.335303; rows[99].acc_z = 10.897339; rows[99].acc_abs = 10.902960;
    rows[100].timestamp = 1000; rows[100].acc_x = 0.943638; rows[100].acc_y = 1.082549; rows[100].acc_z = 11.543994; rows[100].acc_abs = 11.632977;
    rows[101].timestamp = 1010; rows[101].acc_x = 1.686094; rows[101].acc_y = 1.633403; rows[101].acc_z = 11.912827; rows[101].acc_abs = 12.141926;
    rows[102].timestamp = 1020; rows[102].acc_x = 1.892065; rows[102].acc_y = 2.121987; rows[102].acc_z = 12.094849; rows[102].acc_abs = 12.424496;
    rows[103].timestamp = 1030; rows[103].acc_x = 1.599873; rows[103].acc_y = 2.366279; rows[103].acc_z = 12.391831; rows[103].acc_abs = 12.716775;
    rows[104].timestamp = 1040; rows[104].acc_x = 1.154399; rows[104].acc_y = 2.313589; rows[104].acc_z = 12.583432; rows[104].acc_abs = 12.846326;
    rows[105].timestamp = 1050; rows[105].acc_x = 0.713716; rows[105].acc_y = 2.016606; rows[105].acc_z = 12.784615; rows[105].acc_abs = 12.962348;
    rows[106].timestamp = 1060; rows[106].acc_x = 0.340093; rows[106].acc_y = 1.508862; rows[106].acc_z = 12.808564; rows[106].acc_abs = 12.901614;
    rows[107].timestamp = 1070; rows[107].acc_x = 0.349673; rows[107].acc_y = 1.111289; rows[107].acc_z = 12.712764; rows[107].acc_abs = 12.766033;
    rows[108].timestamp = 1080; rows[108].acc_x = 0.565225; rows[108].acc_y = 0.924478; rows[108].acc_z = 12.492422; rows[108].acc_abs = 12.539328;
    rows[109].timestamp = 1090; rows[109].acc_x = 0.819097; rows[109].acc_y = 0.847837; rows[109].acc_z = 12.262500; rows[109].acc_abs = 12.319036;
    rows[110].timestamp = 1100; rows[110].acc_x = 0.934058; rows[110].acc_y = 0.761616; rows[110].acc_z = 12.046948; rows[110].acc_abs = 12.107084;
    rows[111].timestamp = 1110; rows[111].acc_x = 0.938848; rows[111].acc_y = 0.756826; rows[111].acc_z = 11.864926; rows[111].acc_abs = 11.926051;
    rows[112].timestamp = 1120; rows[112].acc_x = 0.814307; rows[112].acc_y = 0.890947; rows[112].acc_z = 11.668535; rows[112].acc_abs = 11.730797;
    rows[113].timestamp = 1130; rows[113].acc_x = 0.574805; rows[113].acc_y = 1.092129; rows[113].acc_z = 11.500884; rows[113].acc_abs = 11.566913;
    rows[114].timestamp = 1140; rows[114].acc_x = 0.239502; rows[114].acc_y = 1.350791; rows[114].acc_z = 11.361973; rows[114].acc_abs = 11.444493;
    rows[115].timestamp = 1150; rows[115].acc_x = -0.162861; rows[115].acc_y = 1.484912; rows[115].acc_z = 11.223062; rows[115].acc_abs = 11.322040;
    rows[116].timestamp = 1160; rows[116].acc_x = -0.589175; rows[116].acc_y = 1.475332; rows[116].acc_z = 11.069780; rows[116].acc_abs = 11.183191;
    rows[117].timestamp = 1170; rows[117].acc_x = -0.905317; rows[117].acc_y = 1.374741; rows[117].acc_z = 10.863809; rows[117].acc_abs = 10.987805;
    rows[118].timestamp = 1180; rows[118].acc_x = -1.092129; rows[118].acc_y = 1.231040; rows[118].acc_z = 10.547666; rows[118].acc_abs = 10.675273;
    rows[119].timestamp = 1190; rows[119].acc_x = -1.140029; rows[119].acc_y = 1.001118; rows[119].acc_z = 10.130933; rows[119].acc_abs = 10.243911;
    rows[120].timestamp = 1200; rows[120].acc_x = -1.154399; rows[120].acc_y = 0.752036; rows[120].acc_z = 9.651929; rows[120].acc_abs = 9.749766;
    rows[121].timestamp = 1210; rows[121].acc_x = -1.211880; rows[121].acc_y = 0.464634; rows[121].acc_z = 9.235195; rows[121].acc_abs = 9.325951;
    rows[122].timestamp = 1220; rows[122].acc_x = -1.312471; rows[122].acc_y = 0.225132; rows[122].acc_z = 8.842412; rows[122].acc_abs = 8.942120;
    rows[123].timestamp = 1230; rows[123].acc_x = -1.413061; rows[123].acc_y = 0.138911; rows[123].acc_z = 8.449629; rows[123].acc_abs = 8.568096;
    rows[124].timestamp = 1240; rows[124].acc_x = -1.508862; rows[124].acc_y = 0.119751; rows[124].acc_z = 8.128696; rows[124].acc_abs = 8.268417;
    rows[125].timestamp = 1250; rows[125].acc_x = -1.633403; rows[125].acc_y = 0.071851; rows[125].acc_z = 7.874824; rows[125].acc_abs = 8.042762;
    rows[126].timestamp = 1260; rows[126].acc_x = -1.777104; rows[126].acc_y = 0.028740; rows[126].acc_z = 7.659273; rows[126].acc_abs = 7.862785;
    rows[127].timestamp = 1270; rows[127].acc_x = -1.930386; rows[127].acc_y = -0.004790; rows[127].acc_z = 7.501201; rows[127].acc_abs = 7.745607;

    float x_mean = 0.0f, y_mean = 0.0f, z_mean = 0.0f, abs_mean = 0.0f;
    float x_min = 100.0f, y_min = 100.0f, z_min = 100.0f, abs_min = 100.0f;
    float x_max = -100.0f, y_max = -100.0f, z_max = -100.0f, abs_max = -100.0f;
    for (int i = 0; i < nrows; ++i) {
        x_mean += rows[i].acc_x;
        y_mean += rows[i].acc_y;
        z_mean += rows[i].acc_z;
        abs_mean += rows[i].acc_abs;

        x_min = std::min(x_min, rows[i].acc_x);
        y_min = std::min(y_min, rows[i].acc_y);
        z_min = std::min(z_min, rows[i].acc_z);
        abs_min = std::min(abs_min, rows[i].acc_abs);
        x_max = std::max(x_max, rows[i].acc_x);
        y_max = std::max(y_max, rows[i].acc_y);
        z_max = std::max(z_max, rows[i].acc_z);
        abs_max = std::max(abs_max, rows[i].acc_abs);
    }
    x_mean /= nrows;
    y_mean /= nrows;
    z_mean /= nrows;
    abs_mean /= nrows;

    std::cout << "x_mean: " << x_mean << std::endl;
    std::cout << "y_mean: " << y_mean << std::endl;
    std::cout << "z_mean: " << z_mean << std::endl;
    std::cout << "abs_mean: " << abs_mean << std::endl;

    std::cout << "x_min: " << x_min << std::endl;
    std::cout << "y_min: " << y_min << std::endl;
    std::cout << "z_min: " << z_min << std::endl;
    std::cout << "abs_min: " << abs_min << std::endl;

    std::cout << "x_max: " << x_max << std::endl;
    std::cout << "y_max: " << y_max << std::endl;
    std::cout << "z_max: " << z_max << std::endl;
    std::cout << "abs_max: " << abs_max << std::endl;

    int nbytes = nrows * sizeof(float);
    float *x_NoDC = (float*) pffft_aligned_malloc(nbytes);
    float *y_NoDC = (float*) pffft_aligned_malloc(nbytes);
    float *z_NoDC = (float*) pffft_aligned_malloc(nbytes);
    float *abs_NoDC = (float*) pffft_aligned_malloc(nbytes);

    float x_std = 0.0f, y_std = 0.0f, z_std = 0.0f, abs_std = 0.0f;
    for (int i = 0; i < nrows; ++i) {
        x_std += (rows[i].acc_x - x_mean) * (rows[i].acc_x - x_mean);
        y_std += (rows[i].acc_y - y_mean) * (rows[i].acc_y - y_mean);
        z_std += (rows[i].acc_z - z_mean) * (rows[i].acc_z - z_mean);
        abs_std += (rows[i].acc_abs - abs_mean) * (rows[i].acc_abs - abs_mean);
        x_NoDC[i] = rows[i].acc_x - x_mean;
        y_NoDC[i] = rows[i].acc_y - y_mean;
        z_NoDC[i] = rows[i].acc_z - z_mean;
        abs_NoDC[i] = rows[i].acc_abs - abs_mean;
    }
    x_std = sqrt(x_std / nrows);
    y_std = sqrt(y_std / nrows);
    z_std = sqrt(z_std / nrows);
    abs_std = sqrt(abs_std / nrows);

    std::cout << "x_std: " << x_std << std::endl;
    std::cout << "y_std: " << y_std << std::endl;
    std::cout << "z_std: " << z_std << std::endl;
    std::cout << "abs_std: " << abs_std << std::endl;

    float *x_fft = (float*) pffft_aligned_malloc(nbytes);
    float *y_fft = (float*) pffft_aligned_malloc(nbytes);
    float *z_fft = (float*) pffft_aligned_malloc(nbytes);
    float *abs_fft = (float*) pffft_aligned_malloc(nbytes);
    pffft_transform(my_setup, x_NoDC, x_fft, NULL, PFFFT_FORWARD);
    pffft_transform(my_setup, y_NoDC, y_fft, NULL, PFFFT_FORWARD);
    pffft_transform(my_setup, z_NoDC, z_fft, NULL, PFFFT_FORWARD);
    pffft_transform(my_setup, abs_NoDC, abs_fft, NULL, PFFFT_FORWARD);

    float *tmp = (float*) pffft_aligned_malloc(nbytes);
    pffft_zreorder(my_setup, x_fft, tmp, PFFFT_FORWARD); memcpy(x_fft, tmp, nbytes);
    pffft_zreorder(my_setup, y_fft, tmp, PFFFT_FORWARD); memcpy(y_fft, tmp, nbytes);
    pffft_zreorder(my_setup, z_fft, tmp, PFFFT_FORWARD); memcpy(z_fft, tmp, nbytes);
    pffft_zreorder(my_setup, abs_fft, tmp, PFFFT_FORWARD); memcpy(abs_fft, tmp, nbytes);

    int fftlen = nrows / 2;
    float x_rfft[fftlen], y_rfft[fftlen], z_rfft[fftlen], abs_rfft[fftlen];
    float x_rfft_sum = 0.0f, y_rfft_sum = 0.0f, z_rfft_sum = 0.0f, abs_rfft_sum = 0.0f;
    float x_rfft_max = 0.0f, y_rfft_max = 0.0f, z_rfft_max = 0.0f, abs_rfft_max = 0.0f;
    int x_rfft_argmax = 0, y_rfft_argmax = 0, z_rfft_argmax = 0, abs_rfft_argmax = 0;

    float x_rfft_psd[fftlen], y_rfft_psd[fftlen], z_rfft_psd[fftlen], abs_rfft_psd[fftlen];
    float x_rfft_psd_sum = 0.0f, y_rfft_psd_sum = 0.0f, z_rfft_psd_sum = 0.0f, abs_rfft_psd_sum = 0.0f;

    for (size_t i = 0; i < fftlen; ++i) {
        // calculate absolute value of fft (hypotenuse of real and imaginary parts)
        x_rfft[i] = std::hypot(x_fft[2*i], x_fft[2*i+1]);
        y_rfft[i] = std::hypot(y_fft[2*i], y_fft[2*i+1]);
        z_rfft[i] = std::hypot(z_fft[2*i], z_fft[2*i+1]);
        abs_rfft[i] = std::hypot(abs_fft[2*i], abs_fft[2*i+1]);

        // calculate sum of fft
        x_rfft_sum += x_rfft[i];
        y_rfft_sum += y_rfft[i];
        z_rfft_sum += z_rfft[i];
        abs_rfft_sum += abs_rfft[i];

        // calculate max and argmax of fft
        if (x_rfft[i] > x_rfft_max) {
            x_rfft_max = x_rfft[i];
            x_rfft_argmax = i;
        }
        if (y_rfft[i] > y_rfft_max) {
            y_rfft_max = y_rfft[i];
            y_rfft_argmax = i;
        }
        if (z_rfft[i] > z_rfft_max) {
            z_rfft_max = z_rfft[i];
            z_rfft_argmax = i;
        }
        if (abs_rfft[i] > abs_rfft_max) {
            abs_rfft_max = abs_rfft[i];
            abs_rfft_argmax = i;
        }

        // calculate power spectral density
        x_rfft_psd[i] = x_rfft[i] * x_rfft[i] / nrows;
        y_rfft_psd[i] = y_rfft[i] * y_rfft[i] / nrows;
        z_rfft_psd[i] = z_rfft[i] * z_rfft[i] / nrows;
        abs_rfft_psd[i] = abs_rfft[i] * abs_rfft[i] / nrows;

        // calculate the sum of the psd
        x_rfft_psd_sum += x_rfft_psd[i];
        y_rfft_psd_sum += y_rfft_psd[i];
        z_rfft_psd_sum += z_rfft_psd[i];
        abs_rfft_psd_sum += abs_rfft_psd[i];
    }

    float x_rfft_peak = x_rfft_argmax * 100.0 / 128.0;
    float y_rfft_peak = y_rfft_argmax * 100.0 / 128.0;
    float z_rfft_peak = z_rfft_argmax * 100.0 / 128.0;
    float abs_rfft_peak = abs_rfft_argmax * 100.0 / 128.0;

    std::cout << "x_rfft_peak: " << x_rfft_peak << std::endl;
    std::cout << "y_rfft_peak: " << y_rfft_peak << std::endl;
    std::cout << "z_rfft_peak: " << z_rfft_peak << std::endl;
    std::cout << "abs_rfft_peak: " << abs_rfft_peak << std::endl;

    float x_rfft_entropy = 0.0f, y_rfft_entropy = 0.0f, z_rfft_entropy = 0.0f, abs_rfft_entropy = 0.0f;
    float x_rfft_pse = 0.0f, y_rfft_pse = 0.0f, z_rfft_pse = 0.0f, abs_rfft_pse = 0.0f;

    for (size_t i = 0; i < fftlen; ++i) {
        // normalize values of fft
        float ix = x_rfft[i] / x_rfft_sum;
        float iy = y_rfft[i] / y_rfft_sum;
        float iz = z_rfft[i] / z_rfft_sum;
        float ia = abs_rfft[i] / abs_rfft_sum;
        // calculate entropy of fft
        x_rfft_entropy += ix * std::log(ix);
        y_rfft_entropy += iy * std::log(iy);
        z_rfft_entropy += iz * std::log(iz);
        abs_rfft_entropy += ia * std::log(ia);

        // normalize values of psd
        float px = x_rfft_psd[i] / x_rfft_psd_sum;
        float py = y_rfft_psd[i] / y_rfft_psd_sum;
        float pz = z_rfft_psd[i] / z_rfft_psd_sum;
        float pabs = abs_rfft_psd[i] / abs_rfft_psd_sum;
        // calculate power spectral entropy
        x_rfft_pse += px * std::log(px);
        y_rfft_pse += py * std::log(py);
        z_rfft_pse += pz * std::log(pz);
        abs_rfft_pse += pabs * std::log(pabs);
    }

    // finalize fft entropy
    x_rfft_entropy = -x_rfft_entropy;
    y_rfft_entropy = -y_rfft_entropy;
    z_rfft_entropy = -z_rfft_entropy;
    abs_rfft_entropy = -abs_rfft_entropy;

    // finalize pse
    x_rfft_pse = -x_rfft_pse;
    y_rfft_pse = -y_rfft_pse;
    z_rfft_pse = -z_rfft_pse;
    abs_rfft_pse = -abs_rfft_pse;

    std::cout << "x_rfft_entropy: " << x_rfft_entropy << std::endl;
    std::cout << "y_rfft_entropy: " << y_rfft_entropy << std::endl;
    std::cout << "z_rfft_entropy: " << z_rfft_entropy << std::endl;
    std::cout << "abs_rfft_entropy: " << abs_rfft_entropy << std::endl;
    std::cout << "x_rfft_pse: " << x_rfft_pse << std::endl;
    std::cout << "y_rfft_pse: " << y_rfft_pse << std::endl;
    std::cout << "z_rfft_pse: " << z_rfft_pse << std::endl;
    std::cout << "abs_rfft_pse: " << abs_rfft_pse << std::endl;

    // Last step
    pffft_aligned_free(x_fft);
    pffft_aligned_free(y_fft);
    pffft_aligned_free(z_fft);
    pffft_aligned_free(abs_fft);
    pffft_aligned_free(x_NoDC);
    pffft_aligned_free(y_NoDC);
    pffft_aligned_free(z_NoDC);
    pffft_aligned_free(abs_NoDC);
    pffft_aligned_free(tmp);
}
