//Maya ASCII 2018ff09 scene
//Name: ball_monster_maya_idle.ma
//Last modified: Fri, Nov 29, 2019 01:56:07 PM
//Codeset: 1252
requires maya "2018ff09";
requires -nodeType "aiOptions" -nodeType "aiAOVDriver" -nodeType "aiAOVFilter" -nodeType "aiStandardSurface"
		 "mtoa" "3.1.1.1";
requires "stereoCamera" "10.0";
currentUnit -l centimeter -a degree -t film;
fileInfo "application" "maya";
fileInfo "product" "Maya 2018";
fileInfo "version" "2018";
fileInfo "cutIdentifier" "201811122215-49253d42f6";
fileInfo "osv" "Microsoft Windows 8 , 64-bit  (Build 9200)\n";
fileInfo "license" "student";
createNode transform -s -n "persp";
	rename -uid "99CDDE12-4AF5-E3DA-A6EE-A8A735E4DAC4";
	setAttr ".v" no;
	setAttr ".t" -type "double3" -857.16766778863166 632.94802915797732 415.56912221464336 ;
	setAttr ".r" -type "double3" 329.6616470828335 1015.8000000013637 3.653868813480675e-15 ;
	setAttr ".rp" -type "double3" 7.1054273576010019e-15 -3.5527136788005009e-15 5.6843418860808015e-14 ;
	setAttr ".rpt" -type "double3" 6.9061113864514906e-14 6.6474565584122951e-15 -1.0207659978831912e-13 ;
createNode camera -s -n "perspShape" -p "persp";
	rename -uid "98C02EBA-4FE5-DF2A-DBDB-31B7B6F764BD";
	setAttr -k off ".v" no;
	setAttr ".rnd" no;
	setAttr ".pze" yes;
	setAttr ".fl" 23.064473657302273;
	setAttr ".coi" 1018.9764898137803;
	setAttr ".ow" 330.25543898768376;
	setAttr ".imn" -type "string" "persp";
	setAttr ".den" -type "string" "persp_depth";
	setAttr ".man" -type "string" "persp_mask";
	setAttr ".tp" -type "double3" 39.213560288962213 31.252608868755161 -73.543478104167832 ;
	setAttr ".hc" -type "string" "viewSet -p %camera";
	setAttr ".ai_translator" -type "string" "perspective";
createNode transform -s -n "top";
	rename -uid "0137014B-44C1-9A8C-7513-D5B9F977D5A2";
	setAttr ".v" no;
	setAttr ".t" -type "double3" 18.759937714750855 1000.1025763973025 5.3901641511434732 ;
	setAttr ".r" -type "double3" -89.999999999999986 0 0 ;
createNode camera -s -n "topShape" -p "top";
	rename -uid "CE8B542D-4B32-26FF-648E-6BAEA4D1E002";
	setAttr -k off ".v" no;
	setAttr ".rnd" no;
	setAttr ".coi" 910.22310938680448;
	setAttr ".ow" 439.28981643933696;
	setAttr ".imn" -type "string" "top";
	setAttr ".den" -type "string" "top_depth";
	setAttr ".man" -type "string" "top_mask";
	setAttr ".tp" -type "double3" 1.9073486328125e-05 89.879467010498047 2.1656837463378906 ;
	setAttr ".hc" -type "string" "viewSet -t %camera";
	setAttr ".o" yes;
	setAttr ".ai_translator" -type "string" "orthographic";
createNode transform -s -n "front";
	rename -uid "572E4D8C-4482-E55D-E40E-8E8EF02E823E";
	setAttr ".v" no;
	setAttr ".t" -type "double3" 37.640381445881538 54.109391008179372 1004.1393451500475 ;
createNode camera -s -n "frontShape" -p "front";
	rename -uid "5F60C286-4411-D734-4E8D-568BB4CA4525";
	setAttr -k off ".v" no;
	setAttr ".rnd" no;
	setAttr ".coi" 1001.9736614037096;
	setAttr ".ow" 464.07066023816463;
	setAttr ".imn" -type "string" "front";
	setAttr ".den" -type "string" "front_depth";
	setAttr ".man" -type "string" "front_mask";
	setAttr ".tp" -type "double3" 1.9073486328125e-05 89.879467010498047 2.1656837463378906 ;
	setAttr ".hc" -type "string" "viewSet -f %camera";
	setAttr ".o" yes;
	setAttr ".ai_translator" -type "string" "orthographic";
createNode transform -s -n "side";
	rename -uid "3B0F08E5-40D7-2E5C-94E7-188BB65D0776";
	setAttr ".v" no;
	setAttr ".t" -type "double3" 1004.1329687496701 46.421501518333294 -64.146224215648147 ;
	setAttr ".r" -type "double3" 0 89.999999999999986 0 ;
createNode camera -s -n "sideShape" -p "side";
	rename -uid "B6DD197C-4000-43EB-65A3-99BD9067EB86";
	setAttr -k off ".v" no;
	setAttr ".rnd" no;
	setAttr ".coi" 1004.1329496761838;
	setAttr ".ow" 290.90974632910269;
	setAttr ".imn" -type "string" "side";
	setAttr ".den" -type "string" "side_depth";
	setAttr ".man" -type "string" "side_mask";
	setAttr ".tp" -type "double3" 1.9073486328125e-05 89.879467010498047 2.1656837463378906 ;
	setAttr ".hc" -type "string" "viewSet -s %camera";
	setAttr ".o" yes;
	setAttr ".ai_translator" -type "string" "orthographic";
createNode transform -n "BallMonster_grp";
	rename -uid "9DBA8745-4876-13C2-54B4-19B25102E0AD";
createNode transform -n "joints_grp" -p "BallMonster_grp";
	rename -uid "91005D86-4729-A5F8-86FF-8495BE66490B";
createNode joint -n "BallMonster_root_jnt" -p "joints_grp";
	rename -uid "D3F4D0EE-4862-917C-0504-8294F2358136";
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
	setAttr ".radi" 1.746379950152122;
createNode joint -n "COG_jnt" -p "BallMonster_root_jnt";
	rename -uid "4B384C5A-4761-2458-371F-17991BBE7D10";
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 89.587698941992372 7.4075499249628498 1;
	setAttr ".radi" 1.746379950152122;
createNode joint -n "torso_jnt" -p "COG_jnt";
	rename -uid "2C8A3472-4D57-3216-3B1E-4C8D3823D5E2";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" -90 75.427084024077104 -90 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 0 -0.24852035943578993 -0.96862667263807356 0 0 0.96862667263807356 -0.24852035943578993 0
		 1 0 0 0 3.1554436208840472e-30 103.58836200667278 23.924519118319651 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "spine2_jnt" -p "torso_jnt";
	rename -uid "728EC900-4EDE-9DE0-1F6A-74B0C215E343";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 1;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" 0 0 2.3725621473481331 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 0 -0.17679935297182423 -0.98424691454367486 0 0 0.98424691454367486 -0.17679935297182423 0
		 1 0 0 0 3.1554436208840472e-30 94.741750526471392 -10.555810226299755 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "spine1_jnt" -p "spine2_jnt";
	rename -uid "1602AC44-482A-59C8-DAEE-8F87158B3246";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 2;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" 0 0 13.685229167949368 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 0 -0.01402609219936013 -0.99990162953043271 0 0 0.99990162953043271 -0.01402609219936013 0
		 1 0 0 0 -2.1774709172168696e-14 89.226451828528056 -41.259631450705726 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "pelvis_jnt" -p "spine1_jnt";
	rename -uid "643AEAE0-484E-18F9-C405-A48DC83C7B2B";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 3;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" 0 0 -20.455283147861127 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 0 -0.36258354494051059 -0.93195127176176573 0 0 0.93195127176176573 -0.36258354494051059 0
		 1 0 0 0 -4.109559298479277e-14 88.816840803454255 -70.460261649301813 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "tailbase_jnt" -p "pelvis_jnt";
	rename -uid "650162E0-4447-69BF-61FC-22A92218C000";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 4;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" 0 0 15.969468613342324 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 0 -0.09218770615345051 -0.99574164663037212 0 0 0.99574164663037212 -0.09218770615345051 0
		 1 0 0 0 -4.5415531443593211e-14 81.214626080634545 -90.000292569072997 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "tail_jnt" -p "tailbase_jnt";
	rename -uid "84237441-4F11-4CF9-B50D-17AC07DABD87";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 5;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 0 -0.09218770615345051 -0.99574164663037212 0 0 0.99574164663037212 -0.09218770615345051 0
		 1 0 0 0 -5.0536948576827475e-14 79.13979967867138 -112.41099291260977 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "L_B_hip_jnt" -p "pelvis_jnt";
	rename -uid "16E32DC3-4268-4B90-DFEE-5D9B588602CB";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 4;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "xzy";
	setAttr ".jo" -type "double3" 160.99773483426716 -5.414189887084552 -69.168526031775983 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 0.094352784833390213 -0.99551111585880858 0.0074276642178708885 0
		 0.32414657083695475 0.037774715290581795 0.9452523850799629 0 -0.9412898345402585 -0.086779543017147465 0.326255664019658 0
		 33.694788136088363 71.915460962963522 -67.041573508302861 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "L_B_knee_jnt" -p "L_B_hip_jnt";
	rename -uid "2E1D28B3-4E6A-B0D3-A00A-70AF78DB65DE";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 5;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "xzy";
	setAttr ".jo" -type "double3" 13.747016192033104 11.166460501849965 -61.570574047173785 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -0.053300382938360952 -0.48075120605947474 -0.87523559516908611 0
		 -5.0494086060448584e-06 -0.87648136255129128 0.48143579122738189 0 -0.99857852427995253 0.025665131454798776 0.04671436475304297 0
		 38.066794165305566 25.786662450307361 -66.697399324139909 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "L_B_ankle_jnt" -p "L_B_knee_jnt";
	rename -uid "DE6AEC10-47D7-5D5F-B7B0-BB9B7672B730";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 6;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" -6.2240047632068496 4.9085983729733105 91.816406597104191 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 0.087127203354737018 -0.85984135402587636 0.503072257575917 0
		 0.1608125826116158 0.51050905847559513 0.84470102076897202 0 -0.98313181400530658 0.0073039113705469956 0.18275253532940833 0
		 37.022815491740481 16.370332233745923 -83.840377391637929 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "L_B_toe_jnt" -p "L_B_ankle_jnt";
	rename -uid "3A882B74-4A2D-3738-D6F2-7387FEB389BD";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 7;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 0.087127203354737018 -0.85984135402587636 0.503072257575917 0
		 0.1608125826116158 0.51050905847559513 0.84470102076897202 0 -0.98313181400530658 0.0073039113705469956 0.18275253532940833 0
		 38.701426752036241 -0.19555446250487662 -74.148080109110467 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "R_B_hip_jnt" -p "pelvis_jnt";
	rename -uid "2967E139-49A1-7B89-9821-9DA05CBAE293";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 4;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "xzy";
	setAttr ".jo" -type "double3" 160.99773483426719 -5.4141898870845404 110.83147396822402 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 0.094352487324174159 0.99551114380640948 -0.007427697691473889 0
		 0.32414567028258146 -0.037774566138293109 -0.94525269985867777 0 -0.94129017448011121 0.086779287334698113 -0.3262547512556474 0
		 -33.694799999999972 71.915499999997195 -67.041600000000074 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "R_B_knee_jnt" -p "R_B_hip_jnt";
	rename -uid "45B0ED20-40E3-703B-EBC7-8596864DDABC";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 5;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "xzy";
	setAttr ".jo" -type "double3" 13.747016192033074 11.166460501849921 -61.570574047173785 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -0.053299679102569303 0.4807511399462378 0.87523567434603355 0
		 -5.7693205041953188e-06 0.87648139036535633 -0.48143574058219546 0 -0.99857856184392357 -0.025665419996660763 -0.046713403238438367 0
		 -38.066789522064532 25.786689621745388 -66.697387571046846 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "R_B_ankle_jnt" -p "R_B_knee_jnt";
	rename -uid "23EA13B0-412C-5EEF-0389-919384377840";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 6;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" -6.2240047632068789 4.9085983729732963 91.816406597104233 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 0.087128210623593313 0.85984126759064061 -0.50307223085919883 0
		 0.16081348262665815 -0.51050920948608236 -0.84470075815958678 0 -0.98313157752104419 -0.0073035318863924475 -0.18275382267675888 0
		 -37.022799999999968 16.37029999999725 -83.840400000000329 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "R_B_toe_jnt" -p "R_B_ankle_jnt";
	rename -uid "0B84EA07-40BB-F15B-A960-14AFA861385D";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 7;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 0.087128210623593313 0.85984126759064061 -0.50307223085919883 0
		 0.16081348262665815 -0.51050920948608236 -0.84470075815958678 0 -0.98313157752104419 -0.0073035318863924475 -0.18275382267675888 0
		 -38.701400000086771 -0.19555299999551679 -74.148100000002984 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "head_jnt" -p "torso_jnt";
	rename -uid "E6346B45-4164-EC3F-802C-5295D6E437CA";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 1;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" 0 180.00000000000003 38.993326410131424 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 3.2162452993532732e-16 -0.41633402410319165 0.90921173572168712 0
		 0 0.90921173572168712 0.41633402410319165 0 -1 -1.3390323479827222e-16 2.9242479711317072e-16 0
		 0 104.51228323886798 57.370941287323369 1;
	setAttr ".radi" 2;
	setAttr ".liw" yes;
createNode joint -n "jaw_jnt" -p "head_jnt";
	rename -uid "5362FFF2-4219-B537-5301-A184A6F0D1C4";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 2;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" 0 179.99999999999997 42.005287131818903 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 3.2756014462303662e-16 -0.29907297994566984 -0.95423024090961217 0
		 -2.1523087148277554e-16 0.95423024090961228 -0.29907297994566978 0 1 3.0334419491004091e-16 2.4819805760537044e-16 0
		 5.8022502275913141e-15 99.819279695562258 67.619764708729448 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "tonguebase_jnt" -p "jaw_jnt";
	rename -uid "549A58A7-459B-8AF7-D4FD-11833D8568D8";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 3;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" 179.99999975696554 0 -149.6342239800646 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -1.7382087948380149e-16 -0.22433613089673854 0.97451182669800418 0
		 4.2417511657459944e-09 0.97451182669800429 0.22433613089673854 0 -1 4.1336367159238224e-09 9.5157787535968365e-10 0
		 6.1674734391539587e-15 90.259113199135186 37.116842041897939 1;
	setAttr ".radi" 1.2999512840611112;
	setAttr ".liw" yes;
createNode joint -n "tonguemid_jnt" -p "tonguebase_jnt";
	rename -uid "C9047EEB-4890-1AF5-EB56-3AA2C77DB1C4";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 4;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" 0 0 4.5884136860162634 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 3.3932869944551856e-10 -0.14565878231761908 0.98933488725190932 0
		 4.2281566888941269e-09 0.98933488725190943 0.14565878231761908 0 -1 4.1336367159238224e-09 9.5157787535968365e-10 0
		 3.1977911610131514e-07 84.679409854062072 61.354965778386131 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "tonguetip_jnt" -p "tonguemid_jnt";
	rename -uid "B0FB5C99-4057-9098-A550-C981361A20B2";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 5;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 3.3932869944551856e-10 -0.14565878231761908 0.98933488725190932 0
		 4.2281566888941269e-09 0.98933488725190943 0.14565878231761908 0 -1 4.1336367159238224e-09 9.5157787535968365e-10 0
		 -4.2825716108844572e-08 81.139516348337466 85.398419160642106 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "L_lowerlip2_jnt" -p "jaw_jnt";
	rename -uid "42540214-4865-57B3-8271-3E9F5CEF045A";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 3;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 3.2756014462303662e-16 -0.29907297994566984 -0.95423024090961217 0
		 -2.1523087148277554e-16 0.95423024090961228 -0.29907297994566978 0 1 3.0334419491004091e-16 2.4819805760537044e-16 0
		 38.537800881108012 78.910071323611291 93.277440139180257 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "L_lowerlip1_jnt" -p "jaw_jnt";
	rename -uid "2F830F45-4453-7434-5DAC-159AEA561790";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 3;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 3.2756014462303662e-16 -0.29907297994566984 -0.95423024090961217 0
		 -2.1523087148277554e-16 0.95423024090961228 -0.29907297994566978 0 1 3.0334419491004091e-16 2.4819805760537044e-16 0
		 22.048105564046704 65.407045491430438 102.22090354378437 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "lowerlip" -p "jaw_jnt";
	rename -uid "E6362F7F-41F0-1BAF-2C86-4189D8039EE8";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 3;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 3.2756014462303662e-16 -0.29907297994566984 -0.95423024090961217 0
		 -2.1523087148277554e-16 0.95423024090961228 -0.29907297994566978 0 1 3.0334419491004091e-16 2.4819805760537044e-16 0
		 9.3104548853924579e-16 63.496920152389272 108.46767010514236 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "R_lowerlip2_jnt" -p "jaw_jnt";
	rename -uid "B7FF9115-4CB9-8F2F-13DA-F0AD4F0113A7";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 3;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".jo" -type "double3" 0 0 -180 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -3.2756014462303657e-16 0.29907297994566973 0.95423024090961217 0
		 2.1523087148277559e-16 -0.95423024090961228 0.29907297994566967 0 1 3.0334419491004091e-16 2.4819805760537044e-16 0
		 -38.537799999999997 78.910099999996874 93.277399999999957 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "R_lowerlip1_jnt" -p "jaw_jnt";
	rename -uid "47C92253-4925-E0B0-9DF0-10B8EAB215FD";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 3;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".jo" -type "double3" 0 0 -180 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -3.2756014462303657e-16 0.29907297994566973 0.95423024090961217 0
		 2.1523087148277559e-16 -0.95423024090961228 0.29907297994566967 0 1 3.0334419491004091e-16 2.4819805760537044e-16 0
		 -22.048100000000005 65.406999999996884 102.22099999999995 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "L_upperlip1_jnt" -p "head_jnt";
	rename -uid "00400170-4EC1-B153-1BEA-85A9ECB9ABF8";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 2;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 3.2162452993532732e-16 -0.41633402410319154 0.90921173572168701 0
		 0 0.90921173572168712 0.41633402410319154 0 -1 -1.3390323479827219e-16 2.9242479711317067e-16 0
		 17.555322453526863 112.28845842353591 106.18104754955451 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "L_upperlip2_jnt" -p "head_jnt";
	rename -uid "387C9C30-4188-BC8B-E2A6-A096EFD94E06";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 2;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 3.2162452993532732e-16 -0.41633402410319154 0.90921173572168701 0
		 0 0.90921173572168712 0.41633402410319154 0 -1 -1.3390323479827219e-16 2.9242479711317067e-16 0
		 31.825542046876119 102.03777812991464 98.040736753443113 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "L_lipcorner_jnt" -p "head_jnt";
	rename -uid "A5281FE1-4EF5-AF1F-79DC-24BB9E5700AB";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 2;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 3.2162452993532732e-16 -0.41633402410319154 0.90921173572168701 0
		 0 0.90921173572168712 0.41633402410319154 0 -1 -1.3390323479827219e-16 2.9242479711317067e-16 0
		 37.943752548978232 91.570202395643719 94.749633252566412 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "L_cheek_jnt" -p "head_jnt";
	rename -uid "378AF281-4F15-00CF-737F-58A2B3C7A503";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 2;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 3.2162452993532732e-16 -0.41633402410319154 0.90921173572168701 0
		 0 0.90921173572168712 0.41633402410319154 0 -1 -1.3390323479827219e-16 2.9242479711317067e-16 0
		 43.436549431515658 104.53953326910089 82.392364945614631 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "L_brow_jnt" -p "head_jnt";
	rename -uid "676440D2-4D11-3D41-245F-A6B5BFE169F0";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 2;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 3.2162452993532732e-16 -0.41633402410319154 0.90921173572168701 0
		 0 0.90921173572168712 0.41633402410319154 0 -1 -1.3390323479827219e-16 2.9242479711317067e-16 0
		 16.754699168376511 134.37206687288284 90.486730075793062 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "browmid_jnt" -p "head_jnt";
	rename -uid "6A541B98-4AE4-CE60-212B-729E4DD165B7";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 2;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 3.2162452993532732e-16 -0.41633402410319154 0.90921173572168701 0
		 0 0.90921173572168712 0.41633402410319154 0 -1 -1.3390323479827219e-16 2.9242479711317067e-16 0
		 3.0824083353529932e-14 134.34574538442013 93.75633170988354 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "fluffbase_jnt" -p "head_jnt";
	rename -uid "ED2F38C5-4435-9B30-5160-00ABF9B446E9";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 2;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" -37.629967618458572 -16.200363998817998 94.525547426016615 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -0.27899720675811124 0.9019323759536666 0.32966429565202471 0
		 0.58631532594739655 0.43187230857932973 -0.68536169111029144 0 -0.76052277887014674 0.0020732315196491749 -0.64930794275959369 0
		 -4.4916267569979498 149.76447788397888 62.492162142357301 1;
	setAttr ".radi" 1.7066365712475375;
createNode joint -n "flufftip_jnt" -p "fluffbase_jnt";
	rename -uid "F18EF2EE-47BF-743C-B2A3-8ABE7E25B966";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 3;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -0.27899720675811124 0.9019323759536666 0.32966429565202471 0
		 0.58631532594739655 0.43187230857932973 -0.68536169111029144 0 -0.76052277887014674 0.0020732315196491749 -0.64930794275959369 0
		 -12.336185860907001 175.12409317586912 71.761326811568352 1;
	setAttr ".radi" 1.7066365712475375;
createNode joint -n "R_brow_jnt" -p "head_jnt";
	rename -uid "A3A53F0E-4EE9-16C2-63CA-F1A2771CB3E5";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 2;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".jo" -type "double3" 0 0 180 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -3.2162452993532732e-16 0.41633402410319165 -0.90921173572168701 0
		 -3.9387645111257066e-32 -0.90921173572168712 -0.41633402410319165 0 -1 -1.3390323479827219e-16 2.9242479711317067e-16 0
		 -16.75470000000001 134.37199999999692 90.486699999999985 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "R_cheek_jnt" -p "head_jnt";
	rename -uid "8F92B28B-4B9C-8622-2E0C-95B398FDA39F";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 2;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".jo" -type "double3" 0 0 180 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -3.2162452993532732e-16 0.41633402410319165 -0.90921173572168701 0
		 -3.9387645111257066e-32 -0.90921173572168712 -0.41633402410319165 0 -1 -1.3390323479827219e-16 2.9242479711317067e-16 0
		 -43.436500000000009 104.53999999999691 82.392399999999981 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "R_upperlip1_jnt" -p "head_jnt";
	rename -uid "6EC708AA-40B6-3A0A-C8C4-9D8C92805DF8";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 2;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".jo" -type "double3" 0 0 180 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -3.2162452993532732e-16 0.41633402410319165 -0.90921173572168701 0
		 -3.9387645111257066e-32 -0.90921173572168712 -0.41633402410319165 0 -1 -1.3390323479827219e-16 2.9242479711317067e-16 0
		 -17.555300000000013 112.2879999999969 106.18099999999998 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "R_upperlip2_jnt" -p "head_jnt";
	rename -uid "017DA369-438B-4114-8963-54992467DED6";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 2;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".jo" -type "double3" 0 0 180 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -3.2162452993532732e-16 0.41633402410319165 -0.90921173572168701 0
		 -3.9387645111257066e-32 -0.90921173572168712 -0.41633402410319165 0 -1 -1.3390323479827219e-16 2.9242479711317067e-16 0
		 -31.825500000000009 102.0379999999969 98.040699999999987 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "R_lipcorner_jnt" -p "head_jnt";
	rename -uid "42132F48-4329-4394-1BC7-E5BA50AD1059";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 2;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".jo" -type "double3" 0 0 180 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -3.2162452993532732e-16 0.41633402410319165 -0.90921173572168701 0
		 -3.9387645111257066e-32 -0.90921173572168712 -0.41633402410319165 0 -1 -1.3390323479827219e-16 2.9242479711317067e-16 0
		 -37.943800000000003 91.570199999996888 94.749599999999973 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "upperlip_jnt" -p "head_jnt";
	rename -uid "F28F5B36-4177-A7D9-08EC-7980F04A957A";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 3.2162452993532732e-16 -0.41633402410319154 0.90921173572168701 0
		 0 0.90921173572168712 0.41633402410319154 0 -1 -1.3390323479827219e-16 2.9242479711317067e-16 0
		 1.5052423220269108e-14 114.71049666245575 108.01016618910072 1;
	setAttr ".radi" 2;
	setAttr ".liw" yes;
createNode joint -n "L_scapula_jnt" -p "torso_jnt";
	rename -uid "EC6A5630-468B-AD74-45EB-DC8AA93F9ED3";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 1;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "xzy";
	setAttr ".jo" -type "double3" -152.4922101199727 -30.055572292018461 -90.044613371312025 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 0.50083973958180883 -0.83821742068891358 0.21575706456394683 0
		 -0.39976628965032429 -0.0029267311505781768 0.91661243058556952 0 -0.76768904441501529 -0.54532833219849985 -0.33655689145635337 0
		 36.956438967404921 141.61327092594567 11.828070710167784 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "L_F_shoulder_jnt" -p "L_scapula_jnt";
	rename -uid "9A5D818B-43EB-2A61-A081-BAA88BEF9DB3";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 2;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" 11.602743583762228 -41.576349347728538 28.174784525555523 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -0.32038344736618551 -0.91567220644197655 0.24269086716117147 0
		 -0.72607920462701547 0.40191788564668807 0.55791666206118695 0 -0.60841058115548885 0.0025344717355741397 -0.79361838511406868 0
		 79.567289377625599 70.298727966476633 30.184425600774276 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "L_F_elbow_jnt" -p "L_F_shoulder_jnt";
	rename -uid "5BB0AC9E-412D-705B-6EA6-2FA28DD8B453";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 3;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" -19.140762731015197 20.02817416409134 20.808628303692565 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -0.31534318438519637 -0.67090340149173244 0.67115370961399579 0
		 -0.28364885690201525 0.7415435189874976 0.60799386133892774 0 -0.90559483326705459 0.0013547377837119243 -0.4241416775625253 0
		 63.103298756368794 23.243804427385662 42.655919348824632 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "L_F_ankle_jnt" -p "L_F_elbow_jnt";
	rename -uid "AA15A758-4E75-701F-1B43-F9B1C6AD7FF8";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 4;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" -1.0232867178996901 5.6198975241531404 31.634389263051538 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -0.32657138926836227 -0.18153446024623926 0.92757553194035836 0
		 -0.091616114410444496 0.98284080842070942 0.16009507451280114 0 -0.94072185862329383 -0.03269839515990941 -0.33759917011494889 0
		 54.239281113312479 4.3853039679465731 61.521455747906003 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "L_F_toe_jnt" -p "L_F_ankle_jnt";
	rename -uid "2EACBF13-42E8-F9CB-CEA2-9493F4E1F9AD";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 5;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -0.32657138926836227 -0.18153446024623926 0.92757553194035836 0
		 -0.091616114410444496 0.98284080842070942 0.16009507451280114 0 -0.94072185862329383 -0.03269839515990941 -0.33759917011494889 0
		 45.998989007271703 -0.19530850730059512 84.926731239824875 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "R_scapula_jnt" -p "torso_jnt";
	rename -uid "D9682450-4901-E051-E97A-96A46C5C520D";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 1;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "xzy";
	setAttr ".jo" -type "double3" -152.49221011997273 -30.055572292018478 89.955386628688004 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 0.50083973958180883 0.83821742068891347 -0.21575706456394669 0
		 -0.39976628965032401 0.002926731150577816 -0.91661243058556963 0 -0.76768904441501529 0.54532833219849985 0.33655689145635304 0
		 -36.956399999999995 141.6129999999969 11.828100000000031 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "R_F_shoulder_jnt" -p "R_scapula_jnt";
	rename -uid "1EADB24E-42AB-ABC3-1549-CBA0A0D89DE2";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 2;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" 11.602743583762289 -41.576349347728545 28.174784525555523 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -0.32038374327208891 0.91567211572107632 -0.24269081881656077 0
		 -0.72607805415682736 -0.40191808657557715 -0.55791801454627954 0 -0.60841179830670589 -0.0025353844512131518 0.79361744909426291 0
		 -79.567299999999975 70.298699999996913 30.184399999999972 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "R_F_elbow_jnt" -p "R_F_shoulder_jnt";
	rename -uid "ABD0F526-4B07-A9B1-8ED0-8EA4C3AC85F0";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 3;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" -19.14076273101535 20.028174164091343 20.808628303692501 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -0.31534264342394935 0.67090356734293299 -0.67115379799634822 0
		 -0.28364738149769475 -0.7415433672693108 -0.60799473470449894 0 -0.90559548376076693 -0.0013556494456625533 0.42414028576248586 0
		 -63.1032903804158 23.243802949243992 42.655898428387943 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "R_F_ankle_jnt" -p "R_F_elbow_jnt";
	rename -uid "952C344B-4CA0-B6B3-2E92-EF825B8F6573";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 4;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jo" -type "double3" -1.0232867178996956 5.6198975241531093 31.63438926305156 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -0.32657155199226562 0.18153441188454014 -0.92757548411495738 0
		 -0.09161615211899074 -0.98284081643321364 -0.16009500374404273 0 -0.94072179846130555 0.032698422815622769 0.33759933507798517 0
		 -54.239299999891585 4.3853000000379474 61.521499999998653 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "R_F_toe_jnt" -p "R_F_ankle_jnt";
	rename -uid "BBF18A73-4651-A992-F23A-469C52B37104";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".oc" 5;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".jot" -type "string" "none";
	setAttr ".jo" -type "double3" 1.4787793334710984e-06 0 0 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" -0.32657155199226562 0.18153441188454014 -0.92757548411495738 0
		 -0.091616176398614205 -0.98284081558928116 -0.16009499503074959 0 -0.9407217960967319 0.032698448182319914 0.33759933920996799 0
		 -45.998971680546894 -0.19530722803366363 84.926691354878429 1;
	setAttr ".radi" 1.7066365712475375;
	setAttr ".liw" yes;
createNode joint -n "torso_splineik_jnt" -p "COG_jnt";
	rename -uid "25FE8CC9-4F59-7F3D-A040-D498E939B547";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".uoc" 1;
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 3.1554436208840472e-30 103.58836364746094 23.924518585205078 1;
	setAttr ".radi" 7;
createNode joint -n "pelvis_splineik_jnt" -p "COG_jnt";
	rename -uid "E155B2E4-459B-238A-179F-0BBA92614CD4";
	addAttr -ci true -sn "liw" -ln "lockInfluenceWeights" -min 0 -max 1 -at "bool";
	setAttr ".mnrl" -type "double3" -360 -360 -360 ;
	setAttr ".mxrl" -type "double3" 360 360 360 ;
	setAttr ".ssc" no;
	setAttr ".bps" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 -4.1095592369626982e-14 88.820426940917969 -70.61297607421875 1;
	setAttr ".radi" 7;
createNode lightLinker -s -n "lightLinker1";
	rename -uid "0B19BFE3-450E-3417-784B-8DB856ADAB00";
	setAttr -s 14 ".lnk";
	setAttr -s 14 ".slnk";
createNode shapeEditorManager -n "shapeEditorManager";
	rename -uid "38870D17-43C7-BC2F-ABB6-CB90BFC228C5";
createNode poseInterpolatorManager -n "poseInterpolatorManager";
	rename -uid "3EBEE79A-46CD-4B0E-7740-C2849D5D157A";
createNode displayLayerManager -n "layerManager";
	rename -uid "299CF9D2-4895-5397-E6F9-CF894517F17D";
	setAttr ".cdl" 3;
	setAttr -s 7 ".dli[1:6]"  1 7 3 5 4 6;
	setAttr -s 4 ".dli";
createNode displayLayer -n "defaultLayer";
	rename -uid "D139C932-4067-40DA-19FA-D4B889BB33B0";
createNode renderLayerManager -n "renderLayerManager";
	rename -uid "376473CC-4844-A077-6A88-CFA89F7AABEC";
createNode renderLayer -n "defaultRenderLayer";
	rename -uid "AE1FFF32-49D2-A175-006F-E7A4D7858858";
	setAttr ".g" yes;
createNode script -n "uiConfigurationScriptNode";
	rename -uid "64B91891-4FD2-D296-7BBF-0D8098ABA5D6";
	setAttr ".b" -type "string" (
		"// Maya Mel UI Configuration File.\n//\n//  This script is machine generated.  Edit at your own risk.\n//\n//\n\nglobal string $gMainPane;\nif (`paneLayout -exists $gMainPane`) {\n\n\tglobal int $gUseScenePanelConfig;\n\tint    $useSceneConfig = $gUseScenePanelConfig;\n\tint    $nodeEditorPanelVisible = stringArrayContains(\"nodeEditorPanel1\", `getPanel -vis`);\n\tint    $nodeEditorWorkspaceControlOpen = (`workspaceControl -exists nodeEditorPanel1Window` && `workspaceControl -q -visible nodeEditorPanel1Window`);\n\tint    $menusOkayInPanels = `optionVar -q allowMenusInPanels`;\n\tint    $nVisPanes = `paneLayout -q -nvp $gMainPane`;\n\tint    $nPanes = 0;\n\tstring $editorName;\n\tstring $panelName;\n\tstring $itemFilterName;\n\tstring $panelConfig;\n\n\t//\n\t//  get current state of the UI\n\t//\n\tsceneUIReplacement -update $gMainPane;\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Top View\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Top View\")) -mbv $menusOkayInPanels  $panelName;\n"
		+ "\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"top\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"smoothShaded\" \n            -activeOnly 0\n            -ignorePanZoom 0\n            -wireframeOnShaded 1\n            -headsUpDisplay 1\n            -holdOuts 1\n            -selectionHiliteDisplay 1\n            -useDefaultMaterial 1\n            -bufferMode \"double\" \n            -twoSidedLighting 0\n            -backfaceCulling 0\n            -xray 1\n            -jointXray 1\n            -activeComponentsXray 0\n            -displayTextures 0\n            -smoothWireframe 0\n            -lineWidth 1\n            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n            -textureDisplay \"modulate\" \n            -textureMaxSize 32768\n            -fogging 0\n            -fogSource \"fragment\" \n            -fogMode \"linear\" \n            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n"
		+ "            -depthOfFieldPreview 1\n            -maxConstantTransparency 1\n            -rendererName \"vp2Renderer\" \n            -objectFilterShowInHUD 1\n            -isFiltered 0\n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n            -lowQualityLighting 0\n            -maximumNumHardwareLights 1\n            -occlusionCulling 0\n            -shadingModel 0\n            -useBaseRenderer 0\n            -useReducedRenderer 0\n            -smallObjectCulling 0\n            -smallObjectThreshold -1 \n            -interactiveDisableShadows 0\n            -interactiveBackFaceCull 0\n            -sortTransparent 1\n            -controllers 1\n            -nurbsCurves 1\n            -nurbsSurfaces 1\n            -polymeshes 1\n            -subdivSurfaces 1\n            -planes 1\n            -lights 1\n            -cameras 1\n            -controlVertices 1\n"
		+ "            -hulls 1\n            -grid 1\n            -imagePlane 1\n            -joints 1\n            -ikHandles 1\n            -deformers 1\n            -dynamics 1\n            -particleInstancers 1\n            -fluids 1\n            -hairSystems 1\n            -follicles 1\n            -nCloths 1\n            -nParticles 1\n            -nRigids 1\n            -dynamicConstraints 1\n            -locators 1\n            -manipulators 1\n            -pluginShapes 1\n            -dimensions 1\n            -handles 1\n            -pivots 1\n            -textures 1\n            -strokes 1\n            -motionTrails 1\n            -clipGhosts 1\n            -greasePencils 1\n            -shadows 0\n            -captureSequenceNumber -1\n            -width 1\n            -height 1\n            -sceneRenderFilter 0\n            $editorName;\n        modelEditor -e -viewSelected 0 $editorName;\n        modelEditor -e \n            -pluginObjects \"gpuCacheDisplayFilter\" 1 \n            $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n"
		+ "\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Side View\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Side View\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"side\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"smoothShaded\" \n            -activeOnly 0\n            -ignorePanZoom 0\n            -wireframeOnShaded 1\n            -headsUpDisplay 1\n            -holdOuts 1\n            -selectionHiliteDisplay 1\n            -useDefaultMaterial 1\n            -bufferMode \"double\" \n            -twoSidedLighting 0\n            -backfaceCulling 0\n            -xray 1\n            -jointXray 1\n            -activeComponentsXray 0\n            -displayTextures 0\n            -smoothWireframe 0\n            -lineWidth 1\n            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n"
		+ "            -textureDisplay \"modulate\" \n            -textureMaxSize 32768\n            -fogging 0\n            -fogSource \"fragment\" \n            -fogMode \"linear\" \n            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n            -depthOfFieldPreview 1\n            -maxConstantTransparency 1\n            -rendererName \"vp2Renderer\" \n            -objectFilterShowInHUD 1\n            -isFiltered 0\n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n            -lowQualityLighting 0\n            -maximumNumHardwareLights 1\n            -occlusionCulling 0\n            -shadingModel 0\n            -useBaseRenderer 0\n            -useReducedRenderer 0\n            -smallObjectCulling 0\n            -smallObjectThreshold -1 \n            -interactiveDisableShadows 0\n            -interactiveBackFaceCull 0\n"
		+ "            -sortTransparent 1\n            -controllers 1\n            -nurbsCurves 1\n            -nurbsSurfaces 1\n            -polymeshes 1\n            -subdivSurfaces 1\n            -planes 1\n            -lights 1\n            -cameras 1\n            -controlVertices 1\n            -hulls 1\n            -grid 1\n            -imagePlane 1\n            -joints 1\n            -ikHandles 1\n            -deformers 1\n            -dynamics 1\n            -particleInstancers 1\n            -fluids 1\n            -hairSystems 1\n            -follicles 1\n            -nCloths 1\n            -nParticles 1\n            -nRigids 1\n            -dynamicConstraints 1\n            -locators 1\n            -manipulators 1\n            -pluginShapes 1\n            -dimensions 1\n            -handles 1\n            -pivots 1\n            -textures 1\n            -strokes 1\n            -motionTrails 1\n            -clipGhosts 1\n            -greasePencils 1\n            -shadows 0\n            -captureSequenceNumber -1\n            -width 1\n            -height 1\n"
		+ "            -sceneRenderFilter 0\n            $editorName;\n        modelEditor -e -viewSelected 0 $editorName;\n        modelEditor -e \n            -pluginObjects \"gpuCacheDisplayFilter\" 1 \n            $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Front View\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Front View\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"front\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"smoothShaded\" \n            -activeOnly 0\n            -ignorePanZoom 0\n            -wireframeOnShaded 1\n            -headsUpDisplay 1\n            -holdOuts 1\n            -selectionHiliteDisplay 1\n            -useDefaultMaterial 1\n            -bufferMode \"double\" \n            -twoSidedLighting 0\n            -backfaceCulling 0\n"
		+ "            -xray 1\n            -jointXray 1\n            -activeComponentsXray 0\n            -displayTextures 0\n            -smoothWireframe 0\n            -lineWidth 1\n            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n            -textureDisplay \"modulate\" \n            -textureMaxSize 32768\n            -fogging 0\n            -fogSource \"fragment\" \n            -fogMode \"linear\" \n            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n            -depthOfFieldPreview 1\n            -maxConstantTransparency 1\n            -rendererName \"vp2Renderer\" \n            -objectFilterShowInHUD 1\n            -isFiltered 0\n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n            -lowQualityLighting 0\n            -maximumNumHardwareLights 1\n            -occlusionCulling 0\n"
		+ "            -shadingModel 0\n            -useBaseRenderer 0\n            -useReducedRenderer 0\n            -smallObjectCulling 0\n            -smallObjectThreshold -1 \n            -interactiveDisableShadows 0\n            -interactiveBackFaceCull 0\n            -sortTransparent 1\n            -controllers 1\n            -nurbsCurves 1\n            -nurbsSurfaces 1\n            -polymeshes 1\n            -subdivSurfaces 1\n            -planes 1\n            -lights 1\n            -cameras 1\n            -controlVertices 1\n            -hulls 1\n            -grid 1\n            -imagePlane 1\n            -joints 1\n            -ikHandles 1\n            -deformers 1\n            -dynamics 1\n            -particleInstancers 1\n            -fluids 1\n            -hairSystems 1\n            -follicles 1\n            -nCloths 1\n            -nParticles 1\n            -nRigids 1\n            -dynamicConstraints 1\n            -locators 1\n            -manipulators 1\n            -pluginShapes 1\n            -dimensions 1\n            -handles 1\n            -pivots 1\n"
		+ "            -textures 1\n            -strokes 1\n            -motionTrails 1\n            -clipGhosts 1\n            -greasePencils 1\n            -shadows 0\n            -captureSequenceNumber -1\n            -width 1\n            -height 1\n            -sceneRenderFilter 0\n            $editorName;\n        modelEditor -e -viewSelected 0 $editorName;\n        modelEditor -e \n            -pluginObjects \"gpuCacheDisplayFilter\" 1 \n            $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"modelPanel\" (localizedPanelLabel(\"Persp View\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tmodelPanel -edit -l (localizedPanelLabel(\"Persp View\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        modelEditor -e \n            -camera \"persp\" \n            -useInteractiveMode 0\n            -displayLights \"default\" \n            -displayAppearance \"smoothShaded\" \n            -activeOnly 0\n            -ignorePanZoom 0\n"
		+ "            -wireframeOnShaded 0\n            -headsUpDisplay 1\n            -holdOuts 1\n            -selectionHiliteDisplay 1\n            -useDefaultMaterial 1\n            -bufferMode \"double\" \n            -twoSidedLighting 0\n            -backfaceCulling 1\n            -xray 0\n            -jointXray 1\n            -activeComponentsXray 0\n            -displayTextures 0\n            -smoothWireframe 0\n            -lineWidth 1\n            -textureAnisotropic 0\n            -textureHilight 1\n            -textureSampling 2\n            -textureDisplay \"modulate\" \n            -textureMaxSize 32768\n            -fogging 0\n            -fogSource \"fragment\" \n            -fogMode \"linear\" \n            -fogStart 0\n            -fogEnd 100\n            -fogDensity 0.1\n            -fogColor 0.5 0.5 0.5 1 \n            -depthOfFieldPreview 1\n            -maxConstantTransparency 1\n            -rendererName \"vp2Renderer\" \n            -objectFilterShowInHUD 1\n            -isFiltered 0\n            -colorResolution 256 256 \n            -bumpResolution 512 512 \n"
		+ "            -textureCompression 0\n            -transparencyAlgorithm \"frontAndBackCull\" \n            -transpInShadows 0\n            -cullingOverride \"none\" \n            -lowQualityLighting 0\n            -maximumNumHardwareLights 1\n            -occlusionCulling 0\n            -shadingModel 0\n            -useBaseRenderer 0\n            -useReducedRenderer 0\n            -smallObjectCulling 0\n            -smallObjectThreshold -1 \n            -interactiveDisableShadows 0\n            -interactiveBackFaceCull 0\n            -sortTransparent 1\n            -controllers 1\n            -nurbsCurves 1\n            -nurbsSurfaces 1\n            -polymeshes 1\n            -subdivSurfaces 1\n            -planes 1\n            -lights 1\n            -cameras 1\n            -controlVertices 1\n            -hulls 1\n            -grid 1\n            -imagePlane 1\n            -joints 1\n            -ikHandles 1\n            -deformers 1\n            -dynamics 1\n            -particleInstancers 1\n            -fluids 1\n            -hairSystems 1\n            -follicles 1\n"
		+ "            -nCloths 1\n            -nParticles 1\n            -nRigids 1\n            -dynamicConstraints 1\n            -locators 1\n            -manipulators 1\n            -pluginShapes 1\n            -dimensions 1\n            -handles 1\n            -pivots 1\n            -textures 1\n            -strokes 1\n            -motionTrails 1\n            -clipGhosts 1\n            -greasePencils 1\n            -shadows 0\n            -captureSequenceNumber -1\n            -width 1319\n            -height 469\n            -sceneRenderFilter 0\n            $editorName;\n        modelEditor -e -viewSelected 0 $editorName;\n        modelEditor -e \n            -pluginObjects \"gpuCacheDisplayFilter\" 1 \n            $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"outlinerPanel\" (localizedPanelLabel(\"ToggledOutliner\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\toutlinerPanel -edit -l (localizedPanelLabel(\"ToggledOutliner\")) -mbv $menusOkayInPanels  $panelName;\n"
		+ "\t\t$editorName = $panelName;\n        outlinerEditor -e \n            -docTag \"isolOutln_fromSeln\" \n            -showShapes 0\n            -showAssignedMaterials 0\n            -showTimeEditor 1\n            -showReferenceNodes 1\n            -showReferenceMembers 1\n            -showAttributes 0\n            -showConnected 0\n            -showAnimCurvesOnly 0\n            -showMuteInfo 0\n            -organizeByLayer 1\n            -organizeByClip 1\n            -showAnimLayerWeight 1\n            -autoExpandLayers 1\n            -autoExpand 0\n            -showDagOnly 1\n            -showAssets 1\n            -showContainedOnly 1\n            -showPublishedAsConnected 0\n            -showParentContainers 0\n            -showContainerContents 1\n            -ignoreDagHierarchy 0\n            -expandConnections 0\n            -showUpstreamCurves 1\n            -showUnitlessCurves 1\n            -showCompounds 1\n            -showLeafs 1\n            -showNumericAttrsOnly 0\n            -highlightActive 1\n            -autoSelectNewObjects 0\n"
		+ "            -doNotSelectNewObjects 0\n            -dropIsParent 1\n            -transmitFilters 0\n            -setFilter \"defaultSetFilter\" \n            -showSetMembers 1\n            -allowMultiSelection 1\n            -alwaysToggleSelect 0\n            -directSelect 0\n            -isSet 0\n            -isSetMember 0\n            -displayMode \"DAG\" \n            -expandObjects 0\n            -setsIgnoreFilters 1\n            -containersIgnoreFilters 0\n            -editAttrName 0\n            -showAttrValues 0\n            -highlightSecondary 0\n            -showUVAttrsOnly 0\n            -showTextureNodesOnly 0\n            -attrAlphaOrder \"default\" \n            -animLayerFilterOptions \"allAffecting\" \n            -sortOrder \"none\" \n            -longNames 0\n            -niceNames 1\n            -showNamespace 1\n            -showPinIcons 0\n            -mapMotionTrails 0\n            -ignoreHiddenAttribute 0\n            -ignoreOutlinerColor 0\n            -renderFilterVisible 0\n            -renderFilterIndex 0\n            -selectionOrder \"chronological\" \n"
		+ "            -expandAttribute 0\n            $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"outlinerPanel\" (localizedPanelLabel(\"Outliner\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\toutlinerPanel -edit -l (localizedPanelLabel(\"Outliner\")) -mbv $menusOkayInPanels  $panelName;\n\t\t$editorName = $panelName;\n        outlinerEditor -e \n            -showShapes 0\n            -showAssignedMaterials 0\n            -showTimeEditor 1\n            -showReferenceNodes 0\n            -showReferenceMembers 0\n            -showAttributes 0\n            -showConnected 0\n            -showAnimCurvesOnly 0\n            -showMuteInfo 0\n            -organizeByLayer 1\n            -organizeByClip 1\n            -showAnimLayerWeight 1\n            -autoExpandLayers 1\n            -autoExpand 0\n            -showDagOnly 1\n            -showAssets 1\n            -showContainedOnly 1\n            -showPublishedAsConnected 0\n            -showParentContainers 0\n"
		+ "            -showContainerContents 1\n            -ignoreDagHierarchy 0\n            -expandConnections 0\n            -showUpstreamCurves 1\n            -showUnitlessCurves 1\n            -showCompounds 1\n            -showLeafs 1\n            -showNumericAttrsOnly 0\n            -highlightActive 1\n            -autoSelectNewObjects 0\n            -doNotSelectNewObjects 0\n            -dropIsParent 1\n            -transmitFilters 0\n            -setFilter \"defaultSetFilter\" \n            -showSetMembers 1\n            -allowMultiSelection 1\n            -alwaysToggleSelect 0\n            -directSelect 0\n            -displayMode \"DAG\" \n            -expandObjects 0\n            -setsIgnoreFilters 1\n            -containersIgnoreFilters 0\n            -editAttrName 0\n            -showAttrValues 0\n            -highlightSecondary 0\n            -showUVAttrsOnly 0\n            -showTextureNodesOnly 0\n            -attrAlphaOrder \"default\" \n            -animLayerFilterOptions \"allAffecting\" \n            -sortOrder \"none\" \n            -longNames 0\n"
		+ "            -niceNames 1\n            -showNamespace 1\n            -showPinIcons 0\n            -mapMotionTrails 0\n            -ignoreHiddenAttribute 0\n            -ignoreOutlinerColor 0\n            -renderFilterVisible 0\n            $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"graphEditor\" (localizedPanelLabel(\"Graph Editor\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Graph Editor\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showAssignedMaterials 0\n                -showTimeEditor 1\n                -showReferenceNodes 0\n                -showReferenceMembers 0\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -organizeByClip 1\n"
		+ "                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 1\n                -showDagOnly 0\n                -showAssets 1\n                -showContainedOnly 0\n                -showPublishedAsConnected 0\n                -showParentContainers 1\n                -showContainerContents 0\n                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUpstreamCurves 1\n                -showUnitlessCurves 1\n                -showCompounds 0\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 1\n                -doNotSelectNewObjects 0\n                -dropIsParent 1\n                -transmitFilters 1\n                -setFilter \"0\" \n                -showSetMembers 0\n                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -isSet 0\n                -isSetMember 0\n                -displayMode \"DAG\" \n"
		+ "                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                -showPinIcons 1\n                -mapMotionTrails 1\n                -ignoreHiddenAttribute 0\n                -ignoreOutlinerColor 0\n                -renderFilterVisible 0\n                -selectionOrder \"display\" \n                -expandAttribute 1\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"GraphEd\");\n            animCurveEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 1\n"
		+ "                -displayInfinities 0\n                -displayValues 0\n                -autoFit 1\n                -autoFitTime 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n                -showResults \"off\" \n                -showBufferCurves \"off\" \n                -smoothness \"fine\" \n                -resultSamples 1\n                -resultScreenSamples 0\n                -resultUpdate \"delayed\" \n                -showUpstreamCurves 1\n                -showCurveNames 0\n                -showActiveCurveNames 0\n                -clipTime \"on\" \n                -stackedCurves 0\n                -stackedCurvesMin -1\n                -stackedCurvesMax 1\n                -stackedCurvesSpace 0.2\n                -displayNormalized 0\n                -preSelectionHighlight 0\n                -constrainDrag 0\n                -classicMode 1\n                -valueLinesToggle 1\n                -outliner \"graphEditor1OutlineEd\" \n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n"
		+ "\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dopeSheetPanel\" (localizedPanelLabel(\"Dope Sheet\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Dope Sheet\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = ($panelName+\"OutlineEd\");\n            outlinerEditor -e \n                -showShapes 1\n                -showAssignedMaterials 0\n                -showTimeEditor 1\n                -showReferenceNodes 0\n                -showReferenceMembers 0\n                -showAttributes 1\n                -showConnected 1\n                -showAnimCurvesOnly 1\n                -showMuteInfo 0\n                -organizeByLayer 1\n                -organizeByClip 1\n                -showAnimLayerWeight 1\n                -autoExpandLayers 1\n                -autoExpand 0\n                -showDagOnly 0\n                -showAssets 1\n                -showContainedOnly 0\n                -showPublishedAsConnected 0\n                -showParentContainers 1\n"
		+ "                -showContainerContents 0\n                -ignoreDagHierarchy 0\n                -expandConnections 1\n                -showUpstreamCurves 1\n                -showUnitlessCurves 0\n                -showCompounds 1\n                -showLeafs 1\n                -showNumericAttrsOnly 1\n                -highlightActive 0\n                -autoSelectNewObjects 0\n                -doNotSelectNewObjects 1\n                -dropIsParent 1\n                -transmitFilters 0\n                -setFilter \"0\" \n                -showSetMembers 0\n                -allowMultiSelection 1\n                -alwaysToggleSelect 0\n                -directSelect 0\n                -displayMode \"DAG\" \n                -expandObjects 0\n                -setsIgnoreFilters 1\n                -containersIgnoreFilters 0\n                -editAttrName 0\n                -showAttrValues 0\n                -highlightSecondary 0\n                -showUVAttrsOnly 0\n                -showTextureNodesOnly 0\n                -attrAlphaOrder \"default\" \n                -animLayerFilterOptions \"allAffecting\" \n"
		+ "                -sortOrder \"none\" \n                -longNames 0\n                -niceNames 1\n                -showNamespace 1\n                -showPinIcons 0\n                -mapMotionTrails 1\n                -ignoreHiddenAttribute 0\n                -ignoreOutlinerColor 0\n                -renderFilterVisible 0\n                $editorName;\n\n\t\t\t$editorName = ($panelName+\"DopeSheetEd\");\n            dopeSheetEditor -e \n                -displayKeys 1\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -displayValues 0\n                -autoFit 0\n                -autoFitTime 0\n                -snapTime \"integer\" \n                -snapValue \"none\" \n                -outliner \"dopeSheetPanel1OutlineEd\" \n                -showSummary 1\n                -showScene 0\n                -hierarchyBelow 0\n                -showTicks 1\n                -selectionWindow 0 0 0 0 \n                $editorName;\n\t\tif (!$useSceneConfig) {\n"
		+ "\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"timeEditorPanel\" (localizedPanelLabel(\"Time Editor\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Time Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"clipEditorPanel\" (localizedPanelLabel(\"Trax Editor\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Trax Editor\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = clipEditorNameFromPanel($panelName);\n            clipEditor -e \n                -displayKeys 0\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -displayValues 0\n                -autoFit 0\n                -autoFitTime 0\n"
		+ "                -snapTime \"none\" \n                -snapValue \"none\" \n                -initialized 0\n                -manageSequencer 0 \n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"sequenceEditorPanel\" (localizedPanelLabel(\"Camera Sequencer\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Camera Sequencer\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = sequenceEditorNameFromPanel($panelName);\n            clipEditor -e \n                -displayKeys 0\n                -displayTangents 0\n                -displayActiveKeys 0\n                -displayActiveKeyTangents 0\n                -displayInfinities 0\n                -displayValues 0\n                -autoFit 0\n                -autoFitTime 0\n                -snapTime \"none\" \n                -snapValue \"none\" \n                -initialized 0\n                -manageSequencer 1 \n                $editorName;\n"
		+ "\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"hyperGraphPanel\" (localizedPanelLabel(\"Hypergraph Hierarchy\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypergraph Hierarchy\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = ($panelName+\"HyperGraphEd\");\n            hyperGraph -e \n                -graphLayoutStyle \"hierarchicalLayout\" \n                -orientation \"horiz\" \n                -mergeConnections 0\n                -zoom 1\n                -animateTransition 0\n                -showRelationships 1\n                -showShapes 0\n                -showDeformers 0\n                -showExpressions 0\n                -showConstraints 0\n                -showConnectionFromSelected 0\n                -showConnectionToSelected 0\n                -showConstraintLabels 0\n                -showUnderworld 0\n                -showInvisible 0\n                -transitionFrames 1\n"
		+ "                -opaqueContainers 0\n                -freeform 0\n                -imagePosition 0 0 \n                -imageScale 1\n                -imageEnabled 0\n                -graphType \"DAG\" \n                -heatMapDisplay 0\n                -updateSelection 1\n                -updateNodeAdded 1\n                -useDrawOverrideColor 0\n                -limitGraphTraversal -1\n                -range 0 0 \n                -iconSize \"smallIcons\" \n                -showCachedConnections 0\n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"hyperShadePanel\" (localizedPanelLabel(\"Hypershade\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Hypershade\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"visorPanel\" (localizedPanelLabel(\"Visor\")) `;\n"
		+ "\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Visor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"nodeEditorPanel\" (localizedPanelLabel(\"Node Editor\")) `;\n\tif ($nodeEditorPanelVisible || $nodeEditorWorkspaceControlOpen) {\n\t\tif (\"\" == $panelName) {\n\t\t\tif ($useSceneConfig) {\n\t\t\t\t$panelName = `scriptedPanel -unParent  -type \"nodeEditorPanel\" -l (localizedPanelLabel(\"Node Editor\")) -mbv $menusOkayInPanels `;\n\n\t\t\t$editorName = ($panelName+\"NodeEditorEd\");\n            nodeEditor -e \n                -allAttributes 0\n                -allNodes 0\n                -autoSizeNodes 1\n                -consistentNameSize 1\n                -createNodeCommand \"nodeEdCreateNodeCommand\" \n                -connectNodeOnCreation 0\n                -connectOnDrop 0\n                -copyConnectionsOnPaste 0\n                -defaultPinnedState 0\n                -additiveGraphingMode 0\n"
		+ "                -settingsChangedCallback \"nodeEdSyncControls\" \n                -traversalDepthLimit -1\n                -keyPressCommand \"nodeEdKeyPressCommand\" \n                -nodeTitleMode \"name\" \n                -gridSnap 0\n                -gridVisibility 1\n                -crosshairOnEdgeDragging 0\n                -popupMenuScript \"nodeEdBuildPanelMenus\" \n                -showNamespace 1\n                -showShapes 1\n                -showSGShapes 0\n                -showTransforms 1\n                -useAssets 1\n                -syncedSelection 1\n                -extendToShapes 1\n                -editorMode \"default\" \n                $editorName;\n\t\t\t}\n\t\t} else {\n\t\t\t$label = `panel -q -label $panelName`;\n\t\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Node Editor\")) -mbv $menusOkayInPanels  $panelName;\n\n\t\t\t$editorName = ($panelName+\"NodeEditorEd\");\n            nodeEditor -e \n                -allAttributes 0\n                -allNodes 0\n                -autoSizeNodes 1\n                -consistentNameSize 1\n                -createNodeCommand \"nodeEdCreateNodeCommand\" \n"
		+ "                -connectNodeOnCreation 0\n                -connectOnDrop 0\n                -copyConnectionsOnPaste 0\n                -defaultPinnedState 0\n                -additiveGraphingMode 0\n                -settingsChangedCallback \"nodeEdSyncControls\" \n                -traversalDepthLimit -1\n                -keyPressCommand \"nodeEdKeyPressCommand\" \n                -nodeTitleMode \"name\" \n                -gridSnap 0\n                -gridVisibility 1\n                -crosshairOnEdgeDragging 0\n                -popupMenuScript \"nodeEdBuildPanelMenus\" \n                -showNamespace 1\n                -showShapes 1\n                -showSGShapes 0\n                -showTransforms 1\n                -useAssets 1\n                -syncedSelection 1\n                -extendToShapes 1\n                -editorMode \"default\" \n                $editorName;\n\t\t\tif (!$useSceneConfig) {\n\t\t\t\tpanel -e -l $label $panelName;\n\t\t\t}\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"createNodePanel\" (localizedPanelLabel(\"Create Node\")) `;\n"
		+ "\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Create Node\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"polyTexturePlacementPanel\" (localizedPanelLabel(\"UV Editor\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"UV Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"renderWindowPanel\" (localizedPanelLabel(\"Render View\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Render View\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"shapePanel\" (localizedPanelLabel(\"Shape Editor\")) `;\n"
		+ "\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tshapePanel -edit -l (localizedPanelLabel(\"Shape Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextPanel \"posePanel\" (localizedPanelLabel(\"Pose Editor\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tposePanel -edit -l (localizedPanelLabel(\"Pose Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dynRelEdPanel\" (localizedPanelLabel(\"Dynamic Relationships\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Dynamic Relationships\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"relationshipPanel\" (localizedPanelLabel(\"Relationship Editor\")) `;\n"
		+ "\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Relationship Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"referenceEditorPanel\" (localizedPanelLabel(\"Reference Editor\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Reference Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"componentEditorPanel\" (localizedPanelLabel(\"Component Editor\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Component Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"dynPaintScriptedPanelType\" (localizedPanelLabel(\"Paint Effects\")) `;\n"
		+ "\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Paint Effects\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"scriptEditorPanel\" (localizedPanelLabel(\"Script Editor\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Script Editor\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"profilerPanel\" (localizedPanelLabel(\"Profiler Tool\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Profiler Tool\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"contentBrowserPanel\" (localizedPanelLabel(\"Content Browser\")) `;\n"
		+ "\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Content Browser\")) -mbv $menusOkayInPanels  $panelName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\t$panelName = `sceneUIReplacement -getNextScriptedPanel \"Stereo\" (localizedPanelLabel(\"Stereo\")) `;\n\tif (\"\" != $panelName) {\n\t\t$label = `panel -q -label $panelName`;\n\t\tscriptedPanel -edit -l (localizedPanelLabel(\"Stereo\")) -mbv $menusOkayInPanels  $panelName;\nstring $editorName = ($panelName+\"Editor\");\n            stereoCameraView -e \n                -camera \"persp\" \n                -useInteractiveMode 0\n                -displayLights \"default\" \n                -displayAppearance \"wireframe\" \n                -activeOnly 0\n                -ignorePanZoom 0\n                -wireframeOnShaded 0\n                -headsUpDisplay 1\n                -holdOuts 1\n                -selectionHiliteDisplay 1\n                -useDefaultMaterial 0\n                -bufferMode \"double\" \n                -twoSidedLighting 1\n"
		+ "                -backfaceCulling 0\n                -xray 0\n                -jointXray 0\n                -activeComponentsXray 0\n                -displayTextures 0\n                -smoothWireframe 0\n                -lineWidth 1\n                -textureAnisotropic 0\n                -textureHilight 1\n                -textureSampling 2\n                -textureDisplay \"modulate\" \n                -textureMaxSize 32768\n                -fogging 0\n                -fogSource \"fragment\" \n                -fogMode \"linear\" \n                -fogStart 0\n                -fogEnd 100\n                -fogDensity 0.1\n                -fogColor 0.5 0.5 0.5 1 \n                -depthOfFieldPreview 1\n                -maxConstantTransparency 1\n                -objectFilterShowInHUD 1\n                -isFiltered 0\n                -colorResolution 4 4 \n                -bumpResolution 4 4 \n                -textureCompression 0\n                -transparencyAlgorithm \"frontAndBackCull\" \n                -transpInShadows 0\n                -cullingOverride \"none\" \n"
		+ "                -lowQualityLighting 0\n                -maximumNumHardwareLights 0\n                -occlusionCulling 0\n                -shadingModel 0\n                -useBaseRenderer 0\n                -useReducedRenderer 0\n                -smallObjectCulling 0\n                -smallObjectThreshold -1 \n                -interactiveDisableShadows 0\n                -interactiveBackFaceCull 0\n                -sortTransparent 1\n                -controllers 1\n                -nurbsCurves 1\n                -nurbsSurfaces 1\n                -polymeshes 1\n                -subdivSurfaces 1\n                -planes 1\n                -lights 1\n                -cameras 1\n                -controlVertices 1\n                -hulls 1\n                -grid 1\n                -imagePlane 1\n                -joints 1\n                -ikHandles 1\n                -deformers 1\n                -dynamics 1\n                -particleInstancers 1\n                -fluids 1\n                -hairSystems 1\n                -follicles 1\n                -nCloths 1\n"
		+ "                -nParticles 1\n                -nRigids 1\n                -dynamicConstraints 1\n                -locators 1\n                -manipulators 1\n                -pluginShapes 1\n                -dimensions 1\n                -handles 1\n                -pivots 1\n                -textures 1\n                -strokes 1\n                -motionTrails 1\n                -clipGhosts 1\n                -greasePencils 1\n                -shadows 0\n                -captureSequenceNumber -1\n                -width 0\n                -height 0\n                -sceneRenderFilter 0\n                -displayMode \"centerEye\" \n                -viewColor 0 0 0 1 \n                -useCustomBackground 1\n                $editorName;\n            stereoCameraView -e -viewSelected 0 $editorName;\n            stereoCameraView -e \n                -pluginObjects \"gpuCacheDisplayFilter\" 1 \n                $editorName;\n\t\tif (!$useSceneConfig) {\n\t\t\tpanel -e -l $label $panelName;\n\t\t}\n\t}\n\n\n\tif ($useSceneConfig) {\n        string $configName = `getPanel -cwl (localizedPanelLabel(\"Current Layout\"))`;\n"
		+ "        if (\"\" != $configName) {\n\t\t\tpanelConfiguration -edit -label (localizedPanelLabel(\"Current Layout\")) \n\t\t\t\t-userCreated false\n\t\t\t\t-defaultImage \"vacantCell.xP:/\"\n\t\t\t\t-image \"\"\n\t\t\t\t-sc false\n\t\t\t\t-configString \"global string $gMainPane; paneLayout -e -cn \\\"single\\\" -ps 1 100 100 $gMainPane;\"\n\t\t\t\t-removeAllPanels\n\t\t\t\t-ap false\n\t\t\t\t\t(localizedPanelLabel(\"Persp View\")) \n\t\t\t\t\t\"modelPanel\"\n"
		+ "\t\t\t\t\t\"$panelName = `modelPanel -unParent -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels `;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"smoothShaded\\\" \\n    -activeOnly 0\\n    -ignorePanZoom 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -holdOuts 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 1\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 0\\n    -backfaceCulling 1\\n    -xray 0\\n    -jointXray 1\\n    -activeComponentsXray 0\\n    -displayTextures 0\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 32768\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -depthOfFieldPreview 1\\n    -maxConstantTransparency 1\\n    -rendererName \\\"vp2Renderer\\\" \\n    -objectFilterShowInHUD 1\\n    -isFiltered 0\\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -controllers 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -imagePlane 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -particleInstancers 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -pluginShapes 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -motionTrails 1\\n    -clipGhosts 1\\n    -greasePencils 1\\n    -shadows 0\\n    -captureSequenceNumber -1\\n    -width 1319\\n    -height 469\\n    -sceneRenderFilter 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName;\\nmodelEditor -e \\n    -pluginObjects \\\"gpuCacheDisplayFilter\\\" 1 \\n    $editorName\"\n"
		+ "\t\t\t\t\t\"modelPanel -edit -l (localizedPanelLabel(\\\"Persp View\\\")) -mbv $menusOkayInPanels  $panelName;\\n$editorName = $panelName;\\nmodelEditor -e \\n    -cam `findStartUpCamera persp` \\n    -useInteractiveMode 0\\n    -displayLights \\\"default\\\" \\n    -displayAppearance \\\"smoothShaded\\\" \\n    -activeOnly 0\\n    -ignorePanZoom 0\\n    -wireframeOnShaded 0\\n    -headsUpDisplay 1\\n    -holdOuts 1\\n    -selectionHiliteDisplay 1\\n    -useDefaultMaterial 1\\n    -bufferMode \\\"double\\\" \\n    -twoSidedLighting 0\\n    -backfaceCulling 1\\n    -xray 0\\n    -jointXray 1\\n    -activeComponentsXray 0\\n    -displayTextures 0\\n    -smoothWireframe 0\\n    -lineWidth 1\\n    -textureAnisotropic 0\\n    -textureHilight 1\\n    -textureSampling 2\\n    -textureDisplay \\\"modulate\\\" \\n    -textureMaxSize 32768\\n    -fogging 0\\n    -fogSource \\\"fragment\\\" \\n    -fogMode \\\"linear\\\" \\n    -fogStart 0\\n    -fogEnd 100\\n    -fogDensity 0.1\\n    -fogColor 0.5 0.5 0.5 1 \\n    -depthOfFieldPreview 1\\n    -maxConstantTransparency 1\\n    -rendererName \\\"vp2Renderer\\\" \\n    -objectFilterShowInHUD 1\\n    -isFiltered 0\\n    -colorResolution 256 256 \\n    -bumpResolution 512 512 \\n    -textureCompression 0\\n    -transparencyAlgorithm \\\"frontAndBackCull\\\" \\n    -transpInShadows 0\\n    -cullingOverride \\\"none\\\" \\n    -lowQualityLighting 0\\n    -maximumNumHardwareLights 1\\n    -occlusionCulling 0\\n    -shadingModel 0\\n    -useBaseRenderer 0\\n    -useReducedRenderer 0\\n    -smallObjectCulling 0\\n    -smallObjectThreshold -1 \\n    -interactiveDisableShadows 0\\n    -interactiveBackFaceCull 0\\n    -sortTransparent 1\\n    -controllers 1\\n    -nurbsCurves 1\\n    -nurbsSurfaces 1\\n    -polymeshes 1\\n    -subdivSurfaces 1\\n    -planes 1\\n    -lights 1\\n    -cameras 1\\n    -controlVertices 1\\n    -hulls 1\\n    -grid 1\\n    -imagePlane 1\\n    -joints 1\\n    -ikHandles 1\\n    -deformers 1\\n    -dynamics 1\\n    -particleInstancers 1\\n    -fluids 1\\n    -hairSystems 1\\n    -follicles 1\\n    -nCloths 1\\n    -nParticles 1\\n    -nRigids 1\\n    -dynamicConstraints 1\\n    -locators 1\\n    -manipulators 1\\n    -pluginShapes 1\\n    -dimensions 1\\n    -handles 1\\n    -pivots 1\\n    -textures 1\\n    -strokes 1\\n    -motionTrails 1\\n    -clipGhosts 1\\n    -greasePencils 1\\n    -shadows 0\\n    -captureSequenceNumber -1\\n    -width 1319\\n    -height 469\\n    -sceneRenderFilter 0\\n    $editorName;\\nmodelEditor -e -viewSelected 0 $editorName;\\nmodelEditor -e \\n    -pluginObjects \\\"gpuCacheDisplayFilter\\\" 1 \\n    $editorName\"\n"
		+ "\t\t\t\t$configName;\n\n            setNamedPanelLayout (localizedPanelLabel(\"Current Layout\"));\n        }\n\n        panelHistory -e -clear mainPanelHistory;\n        sceneUIReplacement -clear;\n\t}\n\n\ngrid -spacing 5 -size 12 -divisions 5 -displayAxes yes -displayGridLines yes -displayDivisionLines yes -displayPerspectiveLabels no -displayOrthographicLabels no -displayAxesBold yes -perspectiveLabelPosition axis -orthographicLabelPosition edge;\nviewManip -drawCompass 0 -compassAngle 0 -frontParameters \"\" -homeParameters \"\" -selectionLockParameters \"\";\n}\n");
	setAttr ".st" 3;
createNode script -n "sceneConfigurationScriptNode";
	rename -uid "FA9B3B86-41E0-CF48-6FF3-839CF689DE8D";
	setAttr ".b" -type "string" "playbackOptions -min 0 -max 40 -ast 0 -aet 801 ";
	setAttr ".st" 6;
createNode phong -n "MaterialFBXASC032FBXASC03550";
	rename -uid "A902C35B-481C-E0BD-4034-119AA74E0166";
	setAttr ".dc" 1;
	setAttr ".c" -type "float3" 0.588 0.588 0.588 ;
	setAttr ".ambc" -type "float3" 0.588 0.588 0.588 ;
	setAttr ".sc" -type "float3" 0 0 0 ;
	setAttr ".rfl" 1;
	setAttr ".cp" 2;
createNode shadingEngine -n "bodySG";
	rename -uid "B6309840-42C7-E0BD-6F5E-8BABBD4D8B91";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo1";
	rename -uid "769CA4CB-42E9-ACE2-16DC-73AA5A729B19";
createNode displayLayer -n "mesh";
	rename -uid "9F12A13A-4A67-F8BF-A635-C8A436F6EE6D";
	setAttr ".c" 2;
	setAttr ".do" 1;
createNode displayLayer -n "control";
	rename -uid "7E40D213-4CD5-D557-5F59-5D90E639DAEC";
	setAttr ".c" 13;
	setAttr ".do" 2;
createNode phong -n "FBXASC0500FBXASC032FBXASC045FBXASC032Default";
	rename -uid "024EB90A-4CBB-03D5-7564-5E8AC7E4EA94";
	setAttr ".dc" 1;
	setAttr ".c" -type "float3" 0.58823532 0.58823532 0.58823532 ;
	setAttr ".ambc" -type "float3" 0.58823532 0.58823532 0.58823532 ;
	setAttr ".sc" -type "float3" 0.61199999 0.61199999 0.61199999 ;
	setAttr ".rfl" 1;
	setAttr ".cp" 19.698307037353516;
createNode shadingEngine -n "Ha_highSG";
	rename -uid "48A24D4A-499E-A6BF-8CC4-C18CC3DCD604";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo2";
	rename -uid "5B03B0F8-444F-F11B-87A8-DDA886CAF522";
createNode blindDataTemplate -n "blindDataTemplate1";
	rename -uid "1691A205-4B05-5888-F613-D48713B6F360";
	addAttr -ci true -sn "MaxVisibility" -ln "MaxVisibility" -min 0 -max 1 -at "bool";
	setAttr ".tid" 16180;
createNode phong -n "Hair";
	rename -uid "0BBC492C-4539-507E-F3BA-369E55DAC6D9";
	setAttr ".dc" 1;
	setAttr ".c" -type "float3" 0.043137256 0.039215688 0.047058828 ;
	setAttr ".ambc" -type "float3" 0.043137256 0.039215688 0.047058828 ;
	setAttr ".sc" -type "float3" 0.61199999 0.61199999 0.61199999 ;
	setAttr ".rfl" 1;
	setAttr ".cp" 19.698307037353516;
createNode shadingEngine -n "EyebrowsSG";
	rename -uid "80FD2B95-4909-EBD7-B6D5-07B43561D363";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo3";
	rename -uid "C8E3C5A9-42A9-8D45-FA65-E291F7D68CAA";
createNode aiStandardSurface -n "OuterFBXASC032EyeFBXASC032Glass";
	rename -uid "F64316A4-4DCB-5948-5E1E-44A21BDFE70F";
	setAttr ".base" 0;
	setAttr ".specular_roughness" 0;
	setAttr ".specular_IOR" 1.5;
	setAttr ".transmission" 1;
	setAttr ".transmission_depth" 1;
	setAttr ".subsurface_type" 1;
createNode shadingEngine -n "RightOuterEye_lowSG";
	rename -uid "66CBBD77-4A0D-444C-24F2-CC9EA5BB4FC6";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo4";
	rename -uid "BE2D330D-4A00-C368-15EF-99BB81A340CB";
createNode aiStandardSurface -n "CharacterFBXASC032Surface";
	rename -uid "E34A8F6F-45A8-9D3B-71CA-08848EBC1780";
	setAttr ".specular" 0.40000000596046448;
	setAttr ".specular_roughness" 1;
	setAttr ".specular_IOR" 1.5;
	setAttr ".subsurface_type" 1;
	setAttr ".emission" 4;
createNode file -n "Char_BaseColor";
	rename -uid "ED757E5D-4344-5E8C-8CBE-62A0DA8DDF4B";
	setAttr ".ftn" -type "string" "\\\\smb86.sg.digipen.edu\\Courses\\2019Fall-Degree\\prj300f19-a.sg\\Submit\\GAY_Simon_Maximilian\\PRJ300\\sceneassets\\images\\Char_BaseColor.png";
	setAttr ".cs" -type "string" "sRGB";
createNode place2dTexture -n "place2dTexture1";
	rename -uid "1E43D648-4395-06A8-1354-FD90812C7E5C";
createNode file -n "Char_Roughness";
	rename -uid "4CC8E014-4644-E5D0-4E52-D991373D9D5F";
	setAttr ".ftn" -type "string" "\\\\smb86.sg.digipen.edu\\Courses\\2019Fall-Degree\\prj300f19-a.sg\\Submit\\GAY_Simon_Maximilian\\PRJ300\\sceneassets\\images\\Char_Roughness.png";
	setAttr ".cs" -type "string" "sRGB";
createNode place2dTexture -n "place2dTexture2";
	rename -uid "CFBE11AC-4BB2-BE36-B119-BAA0B0CB3E6A";
createNode file -n "Char_Emissive";
	rename -uid "14EB3054-45CD-BF1F-44CD-0BB3FF4CFAF3";
	setAttr ".ftn" -type "string" "\\\\smb86.sg.digipen.edu\\Courses\\2019Fall-Degree\\prj300f19-a.sg\\Submit\\GAY_Simon_Maximilian\\PRJ300\\sceneassets\\images\\Char_Emissive.png";
	setAttr ".cs" -type "string" "sRGB";
createNode place2dTexture -n "place2dTexture3";
	rename -uid "3FF02B0D-4F40-3993-8AFE-8E8199AC0074";
createNode shadingEngine -n "RightEye_lowSG";
	rename -uid "ACAD3007-4E64-3EF2-3F8A-43A629773B2E";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo5";
	rename -uid "BCFF46FC-468A-02D6-C941-7D9FF3D0BB7F";
createNode shadingEngine -n "LeftOuterEye_low001SG";
	rename -uid "13021932-4CA0-02A6-B9ED-AE9976AD2C62";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo6";
	rename -uid "674696F8-4104-AD2F-C8E5-05B0ED7BB1E3";
createNode shadingEngine -n "LeftEye_lowSG";
	rename -uid "984E5490-466E-CD9C-55F6-FEB3EA0CB9F7";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo7";
	rename -uid "3DB2B4A3-4D9D-5E78-5A85-51B77AB2984D";
createNode phong -n "MaterialFBXASC032FBXASC03551";
	rename -uid "92BC5599-4CC5-70D8-D6A6-72B4550F7F52";
	setAttr ".dc" 1;
	setAttr ".c" -type "float3" 0.588 0.588 0.588 ;
	setAttr ".ambc" -type "float3" 0.588 0.588 0.588 ;
	setAttr ".sc" -type "float3" 0 0 0 ;
	setAttr ".rfl" 1;
	setAttr ".cp" 2;
createNode shadingEngine -n "bodySG1";
	rename -uid "BD6A6A10-4905-45A6-7394-87B28E09F96E";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo8";
	rename -uid "EAD8923C-4F20-ABB4-1620-43A5558441F8";
createNode phong -n "MaterialFBXASC032FBXASC03552";
	rename -uid "0C265592-4730-E301-69D8-54A9226B3373";
	setAttr ".dc" 1;
	setAttr ".c" -type "float3" 0.588 0.588 0.588 ;
	setAttr ".ambc" -type "float3" 0.588 0.588 0.588 ;
	setAttr ".sc" -type "float3" 0 0 0 ;
	setAttr ".rfl" 1;
	setAttr ".cp" 2;
createNode shadingEngine -n "bodySG2";
	rename -uid "A1DA4C54-41AE-2128-13EC-488005280C6B";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo9";
	rename -uid "86B10786-4EEE-409F-F9D9-36BFA26731FC";
createNode phong -n "MaterialFBXASC032FBXASC03553";
	rename -uid "736E13B3-4BA2-7F3F-CB64-A5AFF77A5556";
	setAttr ".dc" 1;
	setAttr ".c" -type "float3" 0.588 0.588 0.588 ;
	setAttr ".ambc" -type "float3" 0.588 0.588 0.588 ;
	setAttr ".sc" -type "float3" 0 0 0 ;
	setAttr ".rfl" 1;
	setAttr ".cp" 2;
createNode shadingEngine -n "fluff_lowSG";
	rename -uid "72A33E00-499A-7003-6DA9-30ABE1D29954";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo10";
	rename -uid "C4EA844F-4981-70FF-4C30-BBA7B8711227";
createNode phong -n "MaterialFBXASC032FBXASC03554";
	rename -uid "020C3DFF-49A1-0702-7CC7-AE849F4A460D";
	setAttr ".dc" 1;
	setAttr ".c" -type "float3" 0.588 0.588 0.588 ;
	setAttr ".ambc" -type "float3" 0.588 0.588 0.588 ;
	setAttr ".sc" -type "float3" 0 0 0 ;
	setAttr ".rfl" 1;
	setAttr ".cp" 2;
createNode shadingEngine -n "body_low001SG";
	rename -uid "FD5DE5EA-4828-A2BD-FAF7-C88161DB229F";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo11";
	rename -uid "DFF192A6-44D3-0805-1D7B-45B4DB375453";
createNode aiOptions -s -n "defaultArnoldRenderOptions";
	rename -uid "A467BD9D-4813-55D5-937D-DD81DCD121B2";
	setAttr ".version" -type "string" "3.1.2";
createNode aiAOVFilter -s -n "defaultArnoldFilter";
	rename -uid "DE9E274A-4501-8EDB-53E1-89BE1150A4E4";
	setAttr ".ai_translator" -type "string" "gaussian";
createNode aiAOVDriver -s -n "defaultArnoldDriver";
	rename -uid "22014D1A-4E02-AEFB-F7E7-638DA1B76AF4";
	setAttr ".ai_translator" -type "string" "exr";
createNode aiAOVDriver -s -n "defaultArnoldDisplayDriver";
	rename -uid "0D2BB126-4E6D-821E-39A8-99854DEC3864";
	setAttr ".output_mode" 0;
	setAttr ".ai_translator" -type "string" "maya";
createNode nodeGraphEditorInfo -n "hyperShadePrimaryNodeEditorSavedTabsInfo";
	rename -uid "98AC37F2-480A-706F-B8CE-E0BE532DCBE1";
	setAttr ".tgi[0].tn" -type "string" "Untitled_1";
	setAttr ".tgi[0].vl" -type "double2" -330.95236780151544 -322.61903479931897 ;
	setAttr ".tgi[0].vh" -type "double2" 317.85713022663526 336.90474851737002 ;
createNode phong -n "MaterialFBXASC032FBXASC03555";
	rename -uid "C526FFBB-44F7-8B07-6D5C-EF86164B7BAC";
	setAttr ".dc" 1;
	setAttr ".c" -type "float3" 0.588 0.588 0.588 ;
	setAttr ".ambc" -type "float3" 0.588 0.588 0.588 ;
	setAttr ".sc" -type "float3" 0 0 0 ;
	setAttr ".rfl" 1;
	setAttr ".cp" 2;
createNode shadingEngine -n "ballmonster_lowSG";
	rename -uid "DB303021-4E60-44C9-0556-039FC20B51F9";
	setAttr ".ihi" 0;
	setAttr ".ro" yes;
createNode materialInfo -n "materialInfo12";
	rename -uid "62AB9C48-4028-C913-9B8F-0BB6E9265C1A";
createNode dagPose -n "bindPose1";
	rename -uid "C6BF81D9-479E-9106-7BCE-02A6694A9F85";
	setAttr -s 51 ".wm";
	setAttr ".wm[0]" -type "matrix" 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1;
	setAttr -s 52 ".xm";
	setAttr ".xm[0]" -type "matrix" "xform" 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
		 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 yes;
	setAttr ".xm[1]" -type "matrix" "xform" 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
		 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[2]" -type "matrix" "xform" 1 1 1 0 0 0 0 0 89.587698941992372 7.4075499249628498 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[3]" -type "matrix" "xform" 1 1 1 5.5662653976842944e-19 3.4865662728325208e-16
		 0.0031929756861376591 0 3.1554436208840472e-30 14.00066306468041 16.5169691933568 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 -0.089682471941910369 0.7013965028615331 -0.089682471941910369 0.7013965028615331 1
		 1 1 no;
	setAttr ".xm[4]" -type "matrix" "xform" 1 1 1 2.1022909714833637e-17 -8.2607972050186367e-17
		 0.032009747417074186 0 35.597129749392067 -2.8421709430404007e-14 0 0 0 0 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0.020703031364329837 0.99978566927733448 1 1 1 no;
	setAttr ".xm[5]" -type "matrix" "xform" 1 1 1 -3.6076471850263479e-17 -3.634614991552495e-16
		 -0.075145238892183205 0 31.195242546064911 5.6843418860808015e-14 -2.1774709172168699e-14 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0.1191424682809696 0.99287716876354759 1
		 1 1 no;
	setAttr ".xm[6]" -type "matrix" "xform" 1 1 1 0 0 0 0 29.203502960895356 -2.6372165420930071e-15
		 -1.932088381262407e-14 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 -0.17755953155370824 0.98411006130128942 1
		 1 1 no;
	setAttr ".xm[7]" -type "matrix" "xform" 1 1 1 0 0 0 0 20.966794629543898 -2.0705872510258072e-14
		 -4.3199384588004416e-15 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0.13890925235494136 0.99030511440171365 1
		 1 1 no;
	setAttr ".xm[8]" -type "matrix" "xform" 1 1 1 0 0 0 0 22.50654114887676 2.4205766790464606e-14
		 -5.1214171332342635e-15 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[9]" -type "matrix" "xform" 1 1 1 6.7566581882454638e-06 -2.0578088806150874e-06
		 -4.6191715621224544e-07 0 2.942111456190375 -16.990822502083493 33.694788136088405 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0.80666726681245193 -0.56562500453739484 -0.055239625612793858 0.16218772659347247 1
		 1 1 no;
	setAttr ".xm[10]" -type "matrix" "xform" 1 1 1 1.7929040256710663e-29 -3.2354147913858081e-14
		 -2.3736235006700451e-13 0 46.336799034997938 1.6351623113970274e-14 4.2143308239489515e-15 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0.15176452407234378 0.022018479268787257 -0.51573597858885545 0.84291109625676441 1
		 1 1 no;
	setAttr ".xm[11]" -type "matrix" "xform" 1 1 1 -3.8904151220052343e-06 4.121350827376311e-06
		 6.3813890454182227e-07 0 19.586701183223916 -2.7428080683962979e-15 2.1269423754388869e-15 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 -0.068450348277864559 -0.0092028880712264963 0.71812649639416204 0.69247685293331873 1
		 1 1 no;
	setAttr ".xm[12]" -type "matrix" "xform" 1 1 1 0 0 0 0 19.266213027190894 -4.2779586600240698e-15
		 -1.3328504158703111e-14 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[13]" -type "matrix" "xform" 1 1 1 7.7199827064655746e-06 -2.3511970688755263e-06
		 -5.2777044005949832e-07 0 2.942121990975231 -16.990776516016837 -33.69479999999993 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0.56562500453739484 0.80666726681245193 0.16218772659347225 0.05523962561279383 1
		 1 1 no;
	setAttr ".xm[14]" -type "matrix" "xform" 1 1 1 3.1906211924353671e-19 -3.2015013197802657e-14
		 -2.3487341317580478e-13 0 -46.336809573908354 -3.3403376946239405e-05 -1.5450559992302715e-05 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0.15176452407234339 0.022018479268787139 -0.51573597858885556 0.84291109625676464 1
		 1 1 no;
	setAttr ".xm[15]" -type "matrix" "xform" 1 1 1 -5.5096505610379908e-06 5.8452706230147634e-06
		 9.3881737699458042e-07 0 -19.586760394923367 -3.7406458865518744e-05 -2.1508713729190276e-05 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 -0.068450348277864628 -0.0092028880712267565 0.71812649639416215 0.69247685293331851 1
		 1 1 no;
	setAttr ".xm[16]" -type "matrix" "xform" 1 1 1 0 0 0 0 -19.266184439749601 -1.4150770027754334e-05
		 -3.0973806559586592e-05 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[17]" -type "matrix" "xform" 1 1 1 0 0 0 0 -32.626709853925881 -7.4171821103610807
		 -3.1554436208840472e-30 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 -0.33375196105782562 0.94266092975685356 -5.3671408795108414e-17 -1.5159143921072332e-16 1
		 1 1 no;
	setAttr ".xm[18]" -type "matrix" "xform" 1 1 1 0 0 0 0 11.272207582396712 1.4210854715202004e-14
		 -2.1768317625695381e-15 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 -0.3584110235649578 0.93356389079008362 1.0152957976765475e-16 2.6445712683554047e-16 1
		 1 1 no;
	setAttr ".xm[19]" -type "matrix" "xform" 1 1 1 0 0 0 0 31.965998727681111 1.4165556040010261e-14
		 -1.0105563954696382e-14 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0.26190095457774321 -0.96509475700123193 -2.0468460747978385e-09 5.5545938569695955e-10 1
		 1 1 no;
	setAttr ".xm[20]" -type "matrix" "xform" 1 1 1 0 0 0 0 24.872067298162669 -1.8072109974765186e-15
		 -3.1977911425712636e-07 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0.040030764118440851 0.99919844771901734 1
		 1 1 no;
	setAttr ".xm[21]" -type "matrix" "xform" 1 1 1 0 0 0 0 24.302643818658652 1.8841304825053361e-14
		 3.7085141673023289e-07 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[22]" -type "matrix" "xform" 1 1 1 0 0 0 0 -18.229950551075394 -27.62571639145979
		 38.537800881108005 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[23]" -type "matrix" "xform" 1 1 1 0 0 0 0 -22.725683616303385 -43.185460235760445
		 22.048105564046693 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[24]" -type "matrix" "xform" 1 1 1 0 0 0 0 -28.115270299837377 -46.876398688731548
		 -5.7510108752712981e-15 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[25]" -type "matrix" "xform" 1 1 1 0 0 0 0 -18.229920825387822 -27.625677023041192
		 -38.537800000000004 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 -1 6.123233995736766e-17 1
		 1 1 no;
	setAttr ".xm[26]" -type "matrix" "xform" 1 1 1 0 0 0 0 -22.725762052482651 -43.185532492509871
		 -22.048100000000016 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 -1 6.123233995736766e-17 1
		 1 1 no;
	setAttr ".xm[27]" -type "matrix" "xform" 1 1 1 0 0 0 0 44.835190637629189 30.835997267373177
		 -2.3384958424767523e-14 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[28]" -type "matrix" "xform" 1 1 1 0 0 0 0 41.141235128679 27.391497693986928
		 -17.555322453526848 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[29]" -type "matrix" "xform" 1 1 1 0 0 0 0 38.0076759968712 14.682370520699717
		 -31.825542046876109 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[30]" -type "matrix" "xform" 1 1 1 0 0 0 0 39.373373998453324 3.7949294542849499
		 -37.943752548978217 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[31]" -type "matrix" "xform" 1 1 1 0 0 0 0 22.738426919838904 10.442046047733735
		 -43.436549431515651 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[32]" -type "matrix" "xform" 1 1 1 0 0 0 0 17.677619924957298 40.936095313811634
		 -16.754699168376504 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[33]" -type "matrix" "xform" 1 1 1 0 0 0 0 20.661338633019238 42.273409913134927
		 -2.4178890027850503e-14 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[34]" -type "matrix" "xform" 1 1 1 0 0 0 0 -14.183754193480414 43.275964925394376
		 4.4916267569979453 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 -0.11872526223168281 -0.32502757217341566 0.65745138365836908 0.66934226488454274 1
		 1 1 no;
	setAttr ".xm[35]" -type "matrix" "xform" 1 1 1 0 0 0 0 28.116980793683116 2.7126095506056746e-15
		 2.1776080013453517e-15 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[36]" -type "matrix" "xform" 1 1 1 0 0 0 0 17.677620421150966 40.936021990622976
		 16.754700000000017 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 1 6.123233995736766e-17 1
		 1 1 no;
	setAttr ".xm[37]" -type "matrix" "xform" 1 1 1 0 0 0 0 22.738264475745314 10.442484999275138
		 43.436500000000017 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 1 6.123233995736766e-17 1
		 1 1 no;
	setAttr ".xm[38]" -type "matrix" "xform" 1 1 1 0 0 0 0 41.141382753382729 27.391061093427936
		 17.555300000000027 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 1 6.123233995736766e-17 1
		 1 1 no;
	setAttr ".xm[39]" -type "matrix" "xform" 1 1 1 0 0 0 0 38.00755020814519 14.682556945873444
		 31.825500000000019 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 1 6.123233995736766e-17 1
		 1 1 no;
	setAttr ".xm[40]" -type "matrix" "xform" 1 1 1 0 0 0 0 39.37334476221892 3.7949134319599409
		 37.943800000000017 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 1 6.123233995736766e-17 1
		 1 1 no;
	setAttr ".xm[41]" -type "matrix" "xform" 1 1 1 -2.2399141723642684e-16 -6.5065502439057203e-17
		 -7.4730962145848916e-17 0 2.2669785401953901 39.838154710131505 36.956438967404921 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 -0.70669067704897592 0.62002645845122406 -0.34044701093756813 -0.015852776922918144 1
		 1 1 no;
	setAttr ".xm[42]" -type "matrix" "xform" 1 1 1 -8.8846482132921346e-07 -1.0956824919389104e-07
		 1.1243039546023063e-07 0 85.078812727200471 -8.0598282011885772e-14 -1.022860331953822e-13 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0.17760186097834424 -0.31947571410496839 0.26118625227417769 0.89340617229629604 1
		 1 1 no;
	setAttr ".xm[43]" -type "matrix" "xform" 1 1 1 4.9554772303634252e-30 8.4181365511483995e-14
		 1.9944211435505828e-13 0 51.388393366150176 -1.8380138906937199e-14 1.251293237059672e-14 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 -0.19200040322409073 0.13908296943334103 0.20380216506753493 0.94987180729252918 1
		 1 1 no;
	setAttr ".xm[44]" -type "matrix" "xform" 1 1 1 0.034335220169032558 -5.0723398917390056e-07
		 1.0774830304445836e-07 0 28.109114393380338 -5.9770331361984993e-15 1.3928382854745877e-15 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 -0.021942970564589753 0.044734072556421464 0.27265160780445846 0.96082176784092577 1
		 1 1 no;
	setAttr ".xm[45]" -type "matrix" "xform" 1 1 1 0 0 0 0 25.2327435189653 -9.4990880393351376e-15
		 1.5611553356662841e-14 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 no;
	setAttr ".xm[46]" -type "matrix" "xform" 1 1 1 -2.2399141723642684e-16 -6.5065502439057203e-17
		 -7.4730962145848916e-17 0 2.2670174998968093 39.83788500491157 -36.956399999999995 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 -0.62002645845122406 -0.70669067704897592 -0.015852776922918255 0.34044701093756802 1
		 1 1 no;
	setAttr ".xm[47]" -type "matrix" "xform" 1 1 1 -2.6612708689146199e-06 -3.2819743386261972e-07
		 3.3677180109848138e-07 0 -85.078622067837017 7.0848811482449037e-05 0.00015208840052594041 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0.17760186097834468 -0.31947571410496833 0.26118625227417785 0.89340617229629582 1
		 1 1 no;
	setAttr ".xm[48]" -type "matrix" "xform" 1 1 1 1.5893220413941147e-19 8.501501057386227e-14
		 2.0141623065062281e-13 0 -51.388376334145313 -1.5523140849182937e-05 3.323466174265377e-06 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 -0.19200040322409193 0.13908296943334081 0.20380216506753474 0.94987180729252907 1
		 1 1 no;
	setAttr ".xm[49]" -type "matrix" "xform" 1 1 1 0.034336428234453577 -2.0268551438197496e-06
		 4.695077260106709e-07 0 -28.109151207520913 -3.6305016212878627e-05 3.7504031681123706e-05 0
		 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 -0.021942970564589739 0.044734072556421207 0.27265160780445863 0.96082176784092577 1
		 1 1 no;
	setAttr ".xm[50]" -type "matrix" "xform" 1 1 1 0 0 0 0 -25.232676349202073 6.3773927561072696e-06
		 -5.8069600925136911e-05 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1.2904784139758927e-08 0 0 0.99999999999999989 1
		 1 1 no;
	setAttr ".xm[51]" -type "matrix" "xform" 1 1 1 0 0 0 0 41.79591433524039 30.355167608943965
		 -1.6098319189802103e-15 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 1 1 yes;
	setAttr -s 51 ".m";
	setAttr -s 51 ".p";
	setAttr -s 52 ".g[0:51]" yes yes yes no no no no no no no no no no 
		no no no no no no no no no no no no no no no no no no no no no no no no no no no 
		no no no no no no no no no no no no;
	setAttr ".bp" yes;
createNode displayLayer -n "joints";
	rename -uid "805B2EDB-44D0-1AD0-3FBD-9AB33948F2B8";
	setAttr ".do" 3;
createNode dagPose -n "bindPose2";
	rename -uid "DF977A3F-4287-560A-33CB-5F95B27B42A7";
	setAttr -s 2 ".wm";
	setAttr -s 2 ".xm";
	setAttr ".xm[0]" -type "matrix" "xform" 1 1 1 0 0 0 0 3.1554436208840472e-30
		 103.58836364746094 23.924518585205078 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1
		 1 1 yes;
	setAttr ".xm[1]" -type "matrix" "xform" 1 1 1 0 0 0 0 -4.1095592369626982e-14
		 88.820426940917969 -70.61297607421875 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1
		 1 1 yes;
	setAttr -s 2 ".m";
	setAttr -s 2 ".p";
	setAttr ".bp" yes;
createNode animCurveTU -n "BallMonster_root_jnt_visibility";
	rename -uid "1FFA71A8-4D52-850A-072C-0DB5A83FBE23";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "BallMonster_root_jnt_translateX";
	rename -uid "B43D3C70-4F90-1621-1844-919202549A26";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTL -n "BallMonster_root_jnt_translateY";
	rename -uid "F17DB2AA-4C1D-BB71-CD87-BA8C16F0D2E9";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTL -n "BallMonster_root_jnt_translateZ";
	rename -uid "66D54665-49E7-BEB2-DC78-68B5EFACFFCF";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "BallMonster_root_jnt_rotateX";
	rename -uid "4EABDAA7-4800-DE95-1968-27B03C9D9380";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "BallMonster_root_jnt_rotateY";
	rename -uid "9C12BB6D-49B4-8834-6F98-D3AA9FC68C03";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "BallMonster_root_jnt_rotateZ";
	rename -uid "0D9AC268-456D-28D2-C69F-4A8D9CC116FE";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "BallMonster_root_jnt_scaleX";
	rename -uid "20E5AF38-4D34-7D4D-5ED0-EABD6F65D940";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "BallMonster_root_jnt_scaleY";
	rename -uid "7001A084-49CA-7ADD-A5E7-85BBD339D4FA";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "BallMonster_root_jnt_scaleZ";
	rename -uid "0E45FF2A-4BF7-3AEE-E797-94B52B26564D";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "COG_jnt_visibility";
	rename -uid "B70941D4-4FE0-E8C6-11AA-D5B4C83C97D9";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "COG_jnt_translateX";
	rename -uid "855C5C6E-4ADA-614D-5FF5-6F8B548F4B9F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 -0.09330043409769842 2 -0.33927430589616092
		 3 -0.68703046938736867 4 -1.0856777767303745 5 -1.4843250841052835 6 -1.8320812483242059
		 7 -2.0780551217482537 8 -2.1713555576650592 9 -2.091819038986436 10 -1.8858193307586553
		 11 -1.6022712095648397 12 -1.2900894513648646 13 -0.8945610185537326 14 -0.41914180888777292
		 15 -1.2950192159805081e-09 16 0.36154838219085028 17 0.7126158532889213 18 0.9896770337374784
		 19 1.1292065452295861 20 1.157931075228285 21 1.1507036738670111 22 1.1161885371775977
		 23 1.0630498611467565 24 0.99995184173032947 25 0.63487468796692204 26 -0.20174850253283322
		 27 -1.3175741938016494 28 -2.5202588496024561 29 -3.6174589337646363 30 -4.4168309101121972
		 31 -4.7260312425727546 32 -3.6518397537399498 33 -1.603535503542572 34 -0.34252381190885711
		 35 -0.12254725889880015 36 -0.0046276857565163577 37 0.03891086673461918 38 0.03574435840881806
		 39 0.013548749521294843 40 0;
createNode animCurveTL -n "COG_jnt_translateY";
	rename -uid "A830BBF9-4EB4-D8A1-DCAF-BDA42E0DD103";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 89.587698941992372 1 89.392895823698055
		 2 88.966433085317504 3 88.545230651252567 4 88.13256761806484 5 87.889704071845088
		 6 88.089666304157547 7 88.589533384558919 8 89.239275888186384 9 89.888864390040894
		 10 90.388269465416712 11 90.587461689980231 12 90.440713816532977 13 90.113744477084097
		 14 89.773693057075846 15 89.587698941871651 16 89.54665026745414 17 89.525571218416374
		 18 89.517805252970604 19 89.516695829329933 20 89.520781325945421 21 89.531347265460539
		 22 89.545857822373208 23 89.561777171171002 24 89.576569486331522 25 89.587698942372683
		 26 89.595822975580234 27 89.603101733616967 28 89.609253458155322 29 89.613996390863576
		 30 89.617048773504948 31 89.618128847753283 32 89.613374175697373 33 89.602913896467868
		 34 89.592453617120114 35 89.587698944668816 36 89.587698944344638 37 89.58769894361383
		 38 89.58769894282139 39 89.587698942224463 40 89.587698941992372;
createNode animCurveTL -n "COG_jnt_translateZ";
	rename -uid "626201EF-47B5-6FDD-F025-97804B42BA51";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 7.4075499249628498 1 7.4741517655780454
		 2 7.645914411578552 3 7.8807735449129677 4 8.1366648408134807 5 8.3715239754423401
		 6 8.543286629514462 7 8.6098884816518808 8 8.0174391951986479 9 7.4075499407503118
		 10 7.3781603711902326 11 7.3584596683319869 12 7.3465100594750545 13 7.3403737719204276
		 14 7.3381130329702762 15 7.3377900689367017 16 7.3491583402330374 17 7.4287362472396277
		 18 7.6447334251980017 19 8.4013672251065525 20 9.7639535410414187 21 11.29234982178909
		 22 12.546413514871579 23 13.086002067816098 24 12.922710296050145 25 12.422787943866451
		 26 11.678074857872177 27 10.780410884702238 28 9.8216358709694092 29 8.893589663299533
		 30 8.0881121096678772 31 7.4970430562952233 32 7.2122223471909859 33 7.1638650013921144
		 34 7.1913286673629493 35 7.250167132911387 36 7.2959341857093492 37 7.3271081129672924
		 38 7.3639500269006302 39 7.3946879550430946 40 7.4075499249628498;
createNode animCurveTA -n "COG_jnt_rotateX";
	rename -uid "73CD003C-4565-4570-D742-8697FD99F09F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0.20609842299215997 2 0.47084382698410121
		 3 0.52297447541184094 4 0.49690915119797113 5 0.47084382698410132 6 0.2479566866616747
		 7 -0.24239502204766436 8 -0.73274673075700303 9 -0.9556338710794301 10 -0.87576829391505673
		 11 -0.67781178474313297 12 -0.4242246770454533 13 -0.17746730430381139 14 0 15 0.10557770440055825
		 16 0.18090603121906002 17 0.22988427765363689 18 0.25641174090241992 19 0.26438771816354067
		 20 0.19963244761880747 21 0.045216033242905941 22 -0.13908742907671837 23 -0.29350384345262021
		 24 -0.35825911399735283 25 -0.30625835545574071 26 -0.1822565466257432 27 -0.034254387699616244
		 28 0.089747421130381769 29 0.14174817967199355 30 -0.0071030193560307799 31 -0.37923101692609146
		 32 -0.86299741376716721 33 -1.3467638106082451 34 -1.7188918081783042 35 -1.8677430072063281
		 36 -1.6734977344568696 37 -1.210297468669699 38 -0.65744553853662824 39 -0.19424527274945833
		 40 0;
createNode animCurveTA -n "COG_jnt_rotateY";
	rename -uid "D6596090-4865-E1DE-B511-3E9C08323F43";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 -0.0040599126806535003 2 -0.021319936938896614
		 3 -0.059400502099153284 4 -0.12592203748584746 5 -0.22850497242340309 6 -0.41163880203086367
		 7 -0.67024324009194636 8 -0.93758404392808592 9 -1.1469269708607182 10 -1.2315377782112766
		 11 -1.1820107746735642 12 -1.0336952564696884 13 -0.78698946221354149 14 -0.44229163051901399
		 15 0 16 0.78877027658449872 17 1.8493719177071923 18 2.6961084916017439 19 3.2839585920736534
		 20 3.8701306020059261 21 4.4222726048398178 22 4.9080326840165833 23 5.2950589229774803
		 24 5.5509994051637648 25 5.6435022140166913 26 5.4117434629128907 27 4.8283732954272054
		 28 4.0612508402982339 29 3.2782352262645738 30 2.6471855820648074 31 2.3359610364375318
		 32 2.273659853531131 33 2.2571309682702485 34 2.2558595155578733 35 2.2393306302969918
		 36 2.1770294473905913 37 1.7832291849324682 38 1.0408352469812161 39 0.32228104738700009
		 40 0;
createNode animCurveTA -n "COG_jnt_rotateZ";
	rename -uid "6CEDA950-4547-3F7F-028B-E1A872971B7F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0.00039157340030453408 2 -0.0034515689158006171
		 3 -0.0028705822160383413 4 0.12157507469172898 5 0.24640756951858234 6 0.21377792803959719
		 7 0.13360146690143501 8 0.051805907748457995 9 0.014318972225027657 10 0.012174358592624751
		 11 0.0085983493909363978 12 0.0078075430451121086 13 -0.0024369166327592559 14 -0.033142871977121506
		 15 -0.078247202723496945 16 -0.1316867886074081 17 -0.18739850936437763 18 -0.23931924472992777
		 19 -0.31376056011838599 20 -0.42821116327289133 21 -0.56337363577371935 22 -0.69995055920114679
		 23 -0.81864451513545011 24 -0.90015808515690554 25 -0.92519385084578953 26 -0.88919940754542981
		 27 -0.81072307223392581 28 -0.70511850819804156 29 -0.58773937872454218 30 -0.47393934710018987
		 31 -0.37907207661174941 32 -0.30683750405743054 33 -0.24752619896575156 34 -0.19637775798181084
		 35 -0.14891089411565461 36 -0.10499158941178054 37 -0.066380345759233392 38 -0.034837665047058014
		 39 -0.012124049164298564 40 0;
createNode animCurveTU -n "COG_jnt_scaleX";
	rename -uid "1EA78829-4771-75DE-9EFF-218DB044DD41";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "COG_jnt_scaleY";
	rename -uid "5B2488C0-41AD-5199-01AE-9A94731A8C17";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "COG_jnt_scaleZ";
	rename -uid "FAF84864-47CE-F222-21ED-EDA0C47A7708";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_B_ankle_jnt_visibility";
	rename -uid "92F09672-43EB-714E-7890-F39D1F5C0A43";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "L_B_ankle_jnt_translateX";
	rename -uid "25E606CF-41A5-9C89-3EE7-78A8C5A9B7D7";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 19.586701183223916 1 19.586701183223916
		 2 19.586701183223916 3 19.586701183223916 4 19.586701183223916 5 19.586701183223916
		 6 19.586701183223916 7 19.586701183223916 8 19.586701183223916 9 19.586701183223916
		 10 19.586701183223916 11 19.586701183223916 12 19.586701183223916 13 19.586701183223916
		 14 19.586701183223916 15 19.586701183223916 16 19.586701183223916 17 19.586701183223916
		 18 19.586701183223916 19 19.586701183223916 20 19.586701183223916 21 19.586701183223916
		 22 19.586701183223916 23 19.586701183223916 24 19.586701183223916 25 19.586701183223916
		 26 19.586701183223916 27 19.586701183223916 28 19.586701183223916 29 19.586701183223916
		 30 19.586701183223916 31 19.586701183223916 32 19.586701183223916 33 19.586701183223916
		 34 19.586701183223916 35 19.586701183223916 36 19.586701183223916 37 19.586701183223916
		 38 19.586701183223916 39 19.586701183223916 40 19.586701183223916;
createNode animCurveTL -n "L_B_ankle_jnt_translateY";
	rename -uid "78DA898D-4A9E-A938-DBB1-D6A650EA6B50";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -2.7428080683962979e-15 1 -2.7428080683962979e-15
		 2 -2.7428080683962979e-15 3 -2.7428080683962979e-15 4 -2.7428080683962979e-15 5 -2.7428080683962979e-15
		 6 -2.7428080683962979e-15 7 -2.7428080683962979e-15 8 -2.7428080683962979e-15 9 -2.7428080683962979e-15
		 10 -2.7428080683962979e-15 11 -2.7428080683962979e-15 12 -2.7428080683962979e-15
		 13 -2.7428080683962979e-15 14 -2.7428080683962979e-15 15 -2.7428080683962979e-15
		 16 -2.7428080683962979e-15 17 -2.7428080683962979e-15 18 -2.7428080683962979e-15
		 19 -2.7428080683962979e-15 20 -2.7428080683962979e-15 21 -2.7428080683962979e-15
		 22 -2.7428080683962979e-15 23 -2.7428080683962979e-15 24 -2.7428080683962979e-15
		 25 -2.7428080683962979e-15 26 -2.7428080683962979e-15 27 -2.7428080683962979e-15
		 28 -2.7428080683962979e-15 29 -2.7428080683962979e-15 30 -2.7428080683962979e-15
		 31 -2.7428080683962979e-15 32 -2.7428080683962979e-15 33 -2.7428080683962979e-15
		 34 -2.7428080683962979e-15 35 -2.7428080683962979e-15 36 -2.7428080683962979e-15
		 37 -2.7428080683962979e-15 38 -2.7428080683962979e-15 39 -2.7428080683962979e-15
		 40 -2.7428080683962979e-15;
createNode animCurveTL -n "L_B_ankle_jnt_translateZ";
	rename -uid "4D979ED5-41BF-D405-8A5C-5CBD11B02832";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 2.1269423754388869e-15 1 2.1269423754388869e-15
		 2 2.1269423754388869e-15 3 2.1269423754388869e-15 4 2.1269423754388869e-15 5 2.1269423754388869e-15
		 6 2.1269423754388869e-15 7 2.1269423754388869e-15 8 2.1269423754388869e-15 9 2.1269423754388869e-15
		 10 2.1269423754388869e-15 11 2.1269423754388869e-15 12 2.1269423754388869e-15 13 2.1269423754388869e-15
		 14 2.1269423754388869e-15 15 2.1269423754388869e-15 16 2.1269423754388869e-15 17 2.1269423754388869e-15
		 18 2.1269423754388869e-15 19 2.1269423754388869e-15 20 2.1269423754388869e-15 21 2.1269423754388869e-15
		 22 2.1269423754388869e-15 23 2.1269423754388869e-15 24 2.1269423754388869e-15 25 2.1269423754388869e-15
		 26 2.1269423754388869e-15 27 2.1269423754388869e-15 28 2.1269423754388869e-15 29 2.1269423754388869e-15
		 30 2.1269423754388869e-15 31 2.1269423754388869e-15 32 2.1269423754388869e-15 33 2.1269423754388869e-15
		 34 2.1269423754388869e-15 35 2.1269423754388869e-15 36 2.1269423754388869e-15 37 2.1269423754388869e-15
		 38 2.1269423754388869e-15 39 2.1269423754388869e-15 40 2.1269423754388869e-15;
createNode animCurveTA -n "L_B_ankle_jnt_rotateX";
	rename -uid "E5937AD7-4D11-8456-9E3D-839672063B3E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 3.6569062508923897 1 3.6157231678140214
		 2 3.567756304523662 3 3.551887923353839 4 3.5414695632558479 5 3.508538962257747
		 6 3.4881060537816544 7 3.5331535998863277 8 3.6088302969178616 9 3.6516727118215595
		 10 3.6275366945279099 11 3.5998650318761776 12 3.5987188884916375 13 3.6256588393222922
		 14 3.66169048711166 15 3.6309303650575524 16 3.4268069624645547 17 3.1109052167358264
		 18 2.8537180413221885 19 2.6629620054392285 20 2.4571879528927152 21 2.2670271036675436
		 22 2.0926413146812655 23 1.9458609213304114 24 1.8352238911205978 25 1.6754381826406286
		 26 1.4865113569059467 27 1.3471951651235428 28 1.25827644267349 29 1.2230895049321957
		 30 1.2693114673757655 31 1.3951088534331748 32 1.8806508526395249 33 2.6376828510903993
		 34 3.1057317348316777 35 3.2104233038287648 36 3.2304598655675285 37 3.2943306532917571
		 38 3.4462261233624969 39 3.5925406281639209 40 3.6569062508923897;
createNode animCurveTA -n "L_B_ankle_jnt_rotateY";
	rename -uid "AD09BDB0-4AA2-2431-AC74-85AD6D813831";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 2.7456138146031348 1 2.6957452404807469
		 2 2.4038936231036527 3 1.8603886857488434 4 1.3098077659190888 5 0.9026287592726846
		 6 0.65337653290306164 7 0.53829993959097977 8 0.79227788159939228 9 1.4090790012443877
		 10 2.0737078639606916 11 2.567159243169205 12 2.8225958610489856 13 2.9360532505106027
		 14 2.9405369561735286 15 2.7911009588809299 16 2.2210816164716434 17 1.2929944703819873
		 18 0.50770117782569002 19 -0.23736306347505895 20 -1.2753169807267277 21 -2.4046654639886031
		 22 -3.4445132548772706 23 -4.2270214785701254 24 -4.6533389170483037 25 -4.976577782781149
		 26 -5.2167246114877814 27 -5.2384323942993349 28 -5.1301238607377524 29 -4.975023852680633
		 30 -4.9226306595069982 31 -5.0295249730548406 32 -4.3141016356569732 33 -2.7482621250439387
		 34 -1.8291336103934379 35 -1.7101385663118633 36 -1.3146969160205531 37 -0.36219632162835613
		 38 1.0070367465838448 39 2.2215762066118931 40 2.7456138146031348;
createNode animCurveTA -n "L_B_ankle_jnt_rotateZ";
	rename -uid "0D9F3B89-41D8-782A-0EFA-36A9F988ECDD";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -12.174886499913409 1 -12.412018308807884
		 2 -12.205609121370063 3 -11.199609472796636 4 -10.099854898756879 5 -9.4616476808111454
		 6 -9.0127831769231737 7 -8.2554268520130059 8 -8.2146169906571078 9 -9.2448790333380355
		 10 -10.780054760688637 11 -12.021310082783396 12 -12.586765488950368 13 -12.657668740507939
		 14 -12.453217884226934 15 -12.506957903322776 16 -13.212103083358686 17 -14.149612663742619
		 18 -14.797406288842769 19 -14.783734232396395 20 -14.162068528843745 21 -13.186687075640082
		 22 -12.319129091405131 23 -11.911764991644997 24 -12.086119157560212 25 -12.947581929864276
		 26 -14.217553216232195 27 -15.467630342397477 28 -16.526800204286996 29 -17.224271245627992
		 30 -17.087297830043671 31 -16.038675422608623 32 -13.613048888370983 33 -10.417403383293598
		 34 -8.2264655906865389 35 -7.5159911241261845 36 -8.1181530217255098 37 -9.4005330774642299
		 38 -10.687789196293501 39 -11.731843837299913 40 -12.174886499913409;
createNode animCurveTU -n "L_B_ankle_jnt_scaleX";
	rename -uid "C5F75F65-4680-080A-7B65-B7A93F6A5CEF";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_B_ankle_jnt_scaleY";
	rename -uid "F9C3ED9A-4CA3-B388-6328-FC91E74201D2";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_B_ankle_jnt_scaleZ";
	rename -uid "0628C83F-4914-C3A9-FCD1-948A41E4D387";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_B_hip_jnt_visibility";
	rename -uid "D5CE1531-4747-D030-13AB-35B520B6EC81";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "L_B_hip_jnt_translateX";
	rename -uid "2590748E-403B-6629-D7BF-629EFA0BD6F0";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 2.942111456190375 1 2.942111456190375
		 2 2.942111456190375 3 2.942111456190375 4 2.942111456190375 5 2.942111456190375 6 2.942111456190375
		 7 2.942111456190375 8 2.942111456190375 9 2.942111456190375 10 2.942111456190375
		 11 2.942111456190375 12 2.942111456190375 13 2.942111456190375 14 2.942111456190375
		 15 2.942111456190375 16 2.942111456190375 17 2.942111456190375 18 2.942111456190375
		 19 2.942111456190375 20 2.942111456190375 21 2.942111456190375 22 2.942111456190375
		 23 2.942111456190375 24 2.942111456190375 25 2.942111456190375 26 2.942111456190375
		 27 2.942111456190375 28 2.942111456190375 29 2.942111456190375 30 2.942111456190375
		 31 2.942111456190375 32 2.942111456190375 33 2.942111456190375 34 2.942111456190375
		 35 2.942111456190375 36 2.942111456190375 37 2.942111456190375 38 2.942111456190375
		 39 2.942111456190375 40 2.942111456190375;
createNode animCurveTL -n "L_B_hip_jnt_translateY";
	rename -uid "FFEAB9FE-425D-7B40-DF6E-24A2FFD9FD6B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -16.990822502083493 1 -16.990822502083493
		 2 -16.990822502083493 3 -16.990822502083493 4 -16.990822502083493 5 -16.990822502083493
		 6 -16.990822502083493 7 -16.990822502083493 8 -16.990822502083493 9 -16.990822502083493
		 10 -16.990822502083493 11 -16.990822502083493 12 -16.990822502083493 13 -16.990822502083493
		 14 -16.990822502083493 15 -16.990822502083493 16 -16.990822502083493 17 -16.990822502083493
		 18 -16.990822502083493 19 -16.990822502083493 20 -16.990822502083493 21 -16.990822502083493
		 22 -16.990822502083493 23 -16.990822502083493 24 -16.990822502083493 25 -16.990822502083493
		 26 -16.990822502083493 27 -16.990822502083493 28 -16.990822502083493 29 -16.990822502083493
		 30 -16.990822502083493 31 -16.990822502083493 32 -16.990822502083493 33 -16.990822502083493
		 34 -16.990822502083493 35 -16.990822502083493 36 -16.990822502083493 37 -16.990822502083493
		 38 -16.990822502083493 39 -16.990822502083493 40 -16.990822502083493;
createNode animCurveTL -n "L_B_hip_jnt_translateZ";
	rename -uid "A2D396F2-44A4-41EF-0602-42B31EB29863";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 33.694788136088405 1 33.694788136088405
		 2 33.694788136088405 3 33.694788136088405 4 33.694788136088405 5 33.694788136088405
		 6 33.694788136088405 7 33.694788136088405 8 33.694788136088405 9 33.694788136088405
		 10 33.694788136088405 11 33.694788136088405 12 33.694788136088405 13 33.694788136088405
		 14 33.694788136088405 15 33.694788136088405 16 33.694788136088405 17 33.694788136088405
		 18 33.694788136088405 19 33.694788136088405 20 33.694788136088405 21 33.694788136088405
		 22 33.694788136088405 23 33.694788136088405 24 33.694788136088405 25 33.694788136088405
		 26 33.694788136088405 27 33.694788136088405 28 33.694788136088405 29 33.694788136088405
		 30 33.694788136088405 31 33.694788136088405 32 33.694788136088405 33 33.694788136088405
		 34 33.694788136088405 35 33.694788136088405 36 33.694788136088405 37 33.694788136088405
		 38 33.694788136088405 39 33.694788136088405 40 33.694788136088405;
createNode animCurveTA -n "L_B_hip_jnt_rotateX";
	rename -uid "36A129F9-41DA-3E82-E7E4-C19D6178535B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.8424691209241946 1 -1.8497832968078136
		 2 -1.8209832762228368 3 -1.7369109231148505 4 -1.6958457245566865 5 -1.711301571703651
		 6 -1.7806020131967251 7 -1.9331314095264704 8 -2.1898530879809126 9 -2.5001432325979369
		 10 -2.7162310317628475 11 -2.787091270642629 12 -2.7379064109423972 13 -2.5777068560479521
		 14 -2.2940003309872852 15 -1.8450481705222848 16 -0.89789463098691091 17 0.45152250639929092
		 18 1.5611850035711385 19 2.4100061935676513 20 3.341707459643207 21 4.231999649797646
		 22 5.0053551037765187 23 5.6318425137949948 24 6.0708642181325958 25 6.3459405319441222
		 26 6.288004501921006 27 5.8036727613012458 28 5.0821492686950513 29 4.3150675501826825
		 30 3.721927705109334 31 3.4857371441032718 32 3.0906807061753248 33 2.347411516795443
		 34 1.9139922858753771 35 1.8224600015476791 36 1.6164499378726542 37 0.90209554352610821
		 38 -0.28241581421036493 39 -1.3688512283667527 40 -1.8424691209241946;
createNode animCurveTA -n "L_B_hip_jnt_rotateY";
	rename -uid "2BE8AD4F-4CB7-7429-6660-65BEE9381B55";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -2.3098834711131979 1 -2.2835327289744654
		 2 -2.0945571685018316 3 -1.7076694321219394 4 -1.3966503150984932 5 -1.1272118158797775
		 6 -0.73106321805702257 7 -0.36210677410659264 8 -0.39543476410573941 9 -0.74545786319193064
		 10 -1.0924850057773319 11 -1.4361809713536295 12 -1.7382304025366031 13 -2.0193744210674587
		 14 -2.2420455839570557 15 -2.2845868540833831 16 -1.9742518179577722 17 -1.4183368288842133
		 18 -0.90955842433582257 19 -0.19949659078246371 20 0.98021880460843391 21 2.3492855438718983
		 22 3.6186541957827356 23 4.464189436506377 24 4.7738933971452937 25 4.9323860206102017
		 26 5.0518640645145716 27 5.0489410298295869 28 4.9713471576616453 29 4.8498576250340175
		 30 4.7441654940746494 31 4.6499387760363877 32 3.65435414479317 33 1.8602025600791803
		 34 0.78515808019785227 35 0.60425212178628984 36 0.29765313978665109 37 -0.32359442944500327
		 38 -1.1915573153277064 39 -1.9682800874809863 40 -2.3098834711131979;
createNode animCurveTA -n "L_B_hip_jnt_rotateZ";
	rename -uid "08B504EF-41E7-304D-2B27-BAA742153B66";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 11.030126198986382 1 11.214650772752341
		 2 11.460687015690105 3 11.483574112296115 4 11.315063104839899 5 11.05462701157334
		 6 10.529326893732 7 9.6986052651893324 8 9.4968036734195849 9 9.5207017589217742
		 10 9.2635235338868629 11 9.325597760383193 12 9.6619831550123632 13 10.153472424766768
		 14 10.690501301104277 15 11.236594824246547 16 12.084570098736222 17 13.122766513391875
		 18 13.771706828243468 19 13.606167346439548 20 12.814854225865238 21 11.783438598707288
		 22 10.968731447417605 23 10.831861336065552 24 11.342587851105147 25 12.177135005911607
		 26 13.06890878800089 27 13.788000267875582 28 14.339475361201274 29 14.740133569765968
		 30 14.972128913079349 31 15.066458072771313 32 14.602067320565038 33 13.540256661683696
		 34 12.71104264749043 35 12.38294540413902 36 12.354995211361723 37 12.21642091599397
		 38 11.757392572713654 39 11.263757680710999 40 11.030126198986382;
createNode animCurveTU -n "L_B_hip_jnt_scaleX";
	rename -uid "BEF6175A-4BE7-4853-58E4-FA81A1F185FF";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_B_hip_jnt_scaleY";
	rename -uid "510DC482-4DFB-066A-E5D5-398F76BC4136";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_B_hip_jnt_scaleZ";
	rename -uid "CE72CDB5-4DB2-CD61-374F-29A08ABCA7B0";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_B_knee_jnt_visibility";
	rename -uid "4216E414-48F3-8228-DA68-50BDE21C8C10";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "L_B_knee_jnt_translateX";
	rename -uid "A01D3616-4D73-A60C-40D7-929F05720ECB";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 46.336799034997938 1 46.336799034997938
		 2 46.336799034997938 3 46.336799034997938 4 46.336799034997938 5 46.336799034997938
		 6 46.336799034997938 7 46.336799034997938 8 46.336799034997938 9 46.336799034997938
		 10 46.336799034997938 11 46.336799034997938 12 46.336799034997938 13 46.336799034997938
		 14 46.336799034997938 15 46.336799034997938 16 46.336799034997938 17 46.336799034997938
		 18 46.336799034997938 19 46.336799034997938 20 46.336799034997938 21 46.336799034997938
		 22 46.336799034997938 23 46.336799034997938 24 46.336799034997938 25 46.336799034997938
		 26 46.336799034997938 27 46.336799034997938 28 46.336799034997938 29 46.336799034997938
		 30 46.336799034997938 31 46.336799034997938 32 46.336799034997938 33 46.336799034997938
		 34 46.336799034997938 35 46.336799034997938 36 46.336799034997938 37 46.336799034997938
		 38 46.336799034997938 39 46.336799034997938 40 46.336799034997938;
createNode animCurveTL -n "L_B_knee_jnt_translateY";
	rename -uid "64F6B7B1-4CA9-7B03-3595-91892D07F748";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.6351623113970274e-14 1 1.6351623113970274e-14
		 2 1.6351623113970274e-14 3 1.6351623113970274e-14 4 1.6351623113970274e-14 5 1.6351623113970274e-14
		 6 1.6351623113970274e-14 7 1.6351623113970274e-14 8 1.6351623113970274e-14 9 1.6351623113970274e-14
		 10 1.6351623113970274e-14 11 1.6351623113970274e-14 12 1.6351623113970274e-14 13 1.6351623113970274e-14
		 14 1.6351623113970274e-14 15 1.6351623113970274e-14 16 1.6351623113970274e-14 17 1.6351623113970274e-14
		 18 1.6351623113970274e-14 19 1.6351623113970274e-14 20 1.6351623113970274e-14 21 1.6351623113970274e-14
		 22 1.6351623113970274e-14 23 1.6351623113970274e-14 24 1.6351623113970274e-14 25 1.6351623113970274e-14
		 26 1.6351623113970274e-14 27 1.6351623113970274e-14 28 1.6351623113970274e-14 29 1.6351623113970274e-14
		 30 1.6351623113970274e-14 31 1.6351623113970274e-14 32 1.6351623113970274e-14 33 1.6351623113970274e-14
		 34 1.6351623113970274e-14 35 1.6351623113970274e-14 36 1.6351623113970274e-14 37 1.6351623113970274e-14
		 38 1.6351623113970274e-14 39 1.6351623113970274e-14 40 1.6351623113970274e-14;
createNode animCurveTL -n "L_B_knee_jnt_translateZ";
	rename -uid "C96466C0-4239-9688-EFAA-44B848B2BF33";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 4.2143308239489515e-15 1 4.2143308239489515e-15
		 2 4.2143308239489515e-15 3 4.2143308239489515e-15 4 4.2143308239489515e-15 5 4.2143308239489515e-15
		 6 4.2143308239489515e-15 7 4.2143308239489515e-15 8 4.2143308239489515e-15 9 4.2143308239489515e-15
		 10 4.2143308239489515e-15 11 4.2143308239489515e-15 12 4.2143308239489515e-15 13 4.2143308239489515e-15
		 14 4.2143308239489515e-15 15 4.2143308239489515e-15 16 4.2143308239489515e-15 17 4.2143308239489515e-15
		 18 4.2143308239489515e-15 19 4.2143308239489515e-15 20 4.2143308239489515e-15 21 4.2143308239489515e-15
		 22 4.2143308239489515e-15 23 4.2143308239489515e-15 24 4.2143308239489515e-15 25 4.2143308239489515e-15
		 26 4.2143308239489515e-15 27 4.2143308239489515e-15 28 4.2143308239489515e-15 29 4.2143308239489515e-15
		 30 4.2143308239489515e-15 31 4.2143308239489515e-15 32 4.2143308239489515e-15 33 4.2143308239489515e-15
		 34 4.2143308239489515e-15 35 4.2143308239489515e-15 36 4.2143308239489515e-15 37 4.2143308239489515e-15
		 38 4.2143308239489515e-15 39 4.2143308239489515e-15 40 4.2143308239489515e-15;
createNode animCurveTA -n "L_B_knee_jnt_rotateX";
	rename -uid "E976B68F-470E-2A11-337C-51A5FBAA8CEF";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.6420248927795616e-15 1 1.6227173882172041e-15
		 2 1.6361960901589359e-15 3 1.7078570365214209e-15 4 1.7808961832657001e-15 5 1.8125610650359518e-15
		 6 1.8211938835856154e-15 7 1.8497290937864921e-15 8 1.874008323557863e-15 9 1.8166194343901987e-15
		 10 1.6773684783963323e-15 11 1.5752189714932788e-15 12 1.5402522293172517e-15 13 1.5541811339447204e-15
		 14 1.5963438221095313e-15 15 1.6231396726808742e-15 16 1.620809635671992e-15 17 1.6113732108807358e-15
		 18 1.5957842413454203e-15 19 1.5683599372841404e-15 20 1.5369383740211469e-15 21 1.5160578408482923e-15
		 22 1.5054534007010284e-15 23 1.5149037071104565e-15 24 1.5324162733434249e-15 25 1.5184958732039632e-15
		 26 1.4792397403808009e-15 27 1.4364527193948139e-15 28 1.4029370128302992e-15 29 1.3924597604426657e-15
		 30 1.4449360955684347e-15 31 1.5673528250837198e-15 32 1.7718904995350199e-15 33 1.9996119242592788e-15
		 34 2.1469322583954999e-15 35 2.1907182639529845e-15 36 2.1332182991430374e-15 37 2.0017136150855284e-15
		 38 1.8428678548704315e-15 39 1.70327683822623e-15 40 1.6420248927795616e-15;
createNode animCurveTA -n "L_B_knee_jnt_rotateY";
	rename -uid "24D6C3E0-4241-F615-B9E1-0A9D973DD669";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -2.9631433416711883 1 -2.9283016630585146
		 2 -2.952624878917566 3 -3.0819418320317236 4 -3.2137458395765166 5 -3.2708872288422421
		 6 -3.2864657251962095 7 -3.3379594135572828 8 -3.3817729016196716 9 -3.2782108267870469
		 10 -3.0269231971725508 11 -2.8425876048403258 12 -2.7794877884401994 13 -2.8046234250466524
		 14 -2.8807088055131045 15 -2.9290637035137586 16 -2.9248589964602441 17 -2.9078303390919262
		 18 -2.8796990047344475 19 -2.8302100205317844 20 -2.7735077157268728 21 -2.735827402161739
		 22 -2.7166909832483057 23 -2.7337446909216689 24 -2.7653472837063346 25 -2.7402269940152193
		 26 -2.6693866863522775 27 -2.5921746557051923 28 -2.5316933297613842 29 -2.5127864296358497
		 30 -2.6074834733329233 31 -2.8283926194533264 32 -3.1974944831560972 33 -3.6084329691648787
		 34 -3.8742823293713324 35 -3.9532970942488688 36 -3.8495345760177919 37 -3.6122256103151065
		 38 -3.3255778507081808 39 -3.0736765589889044 40 -2.9631433416711883;
createNode animCurveTA -n "L_B_knee_jnt_rotateZ";
	rename -uid "78544FE4-4994-EE62-5564-F6962F089850";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -21.738747966322212 1 -21.483136143757687
		 2 -21.661580517964758 3 -22.610298931950052 4 -23.577263324348127 5 -23.996474316345747
		 6 -24.110764098136112 7 -24.488541405563421 8 -24.809973838857911 9 -24.050203019810692
		 10 -22.206661274657336 11 -20.854305171400814 12 -20.391380888881994 13 -20.575785491074669
		 14 -21.133976816692112 15 -21.488726762734402 16 -21.457879430570912 17 -21.332950715335041
		 18 -21.126568533631787 19 -20.7634984993336 20 -20.347508798150816 21 -20.071071668577986
		 22 -19.930679611978228 23 -20.055792105791472 24 -20.287640761226644 25 -20.103348749849239
		 26 -19.583637275724978 27 -19.017180415336213 28 -18.573466375967502 29 -18.434758156600154
		 30 -19.129491731292983 31 -20.750165352926974 32 -23.458037184874527 33 -26.47283841010605
		 34 -28.423210556219193 35 -29.002892961432234 36 -28.241651613281956 37 -26.500662669881812
		 38 -24.397705545407565 39 -22.549661741361309 40 -21.738747966322212;
createNode animCurveTU -n "L_B_knee_jnt_scaleX";
	rename -uid "9AA796B9-4881-7C99-8A61-A7A5EF5EE483";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_B_knee_jnt_scaleY";
	rename -uid "5F3E1867-4156-B320-C2CC-4C96ADB60BD4";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_B_knee_jnt_scaleZ";
	rename -uid "FF719975-4FE4-1B05-7A47-68A67A5C0019";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_B_toe_jnt_visibility";
	rename -uid "845DB0FC-471A-28A2-5223-618A445B172C";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "L_B_toe_jnt_translateX";
	rename -uid "5CDE890C-40B6-B30A-FC6E-ECAD832D8A36";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 19.266213027190894 1 19.266213027190894
		 2 19.266213027190894 3 19.266213027190894 4 19.266213027190894 5 19.266213027190894
		 6 19.266213027190894 7 19.266213027190894 8 19.266213027190894 9 19.266213027190894
		 10 19.266213027190894 11 19.266213027190894 12 19.266213027190894 13 19.266213027190894
		 14 19.266213027190894 15 19.266213027190894 16 19.266213027190894 17 19.266213027190894
		 18 19.266213027190894 19 19.266213027190894 20 19.266213027190894 21 19.266213027190894
		 22 19.266213027190894 23 19.266213027190894 24 19.266213027190894 25 19.266213027190894
		 26 19.266213027190894 27 19.266213027190894 28 19.266213027190894 29 19.266213027190894
		 30 19.266213027190894 31 19.266213027190894 32 19.266213027190894 33 19.266213027190894
		 34 19.266213027190894 35 19.266213027190894 36 19.266213027190894 37 19.266213027190894
		 38 19.266213027190894 39 19.266213027190894 40 19.266213027190894;
createNode animCurveTL -n "L_B_toe_jnt_translateY";
	rename -uid "80EDC249-49F1-7088-CEEA-5C8CFC095430";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -4.2779586600240698e-15 1 -4.2779586600240698e-15
		 2 -4.2779586600240698e-15 3 -4.2779586600240698e-15 4 -4.2779586600240698e-15 5 -4.2779586600240698e-15
		 6 -4.2779586600240698e-15 7 -4.2779586600240698e-15 8 -4.2779586600240698e-15 9 -4.2779586600240698e-15
		 10 -4.2779586600240698e-15 11 -4.2779586600240698e-15 12 -4.2779586600240698e-15
		 13 -4.2779586600240698e-15 14 -4.2779586600240698e-15 15 -4.2779586600240698e-15
		 16 -4.2779586600240698e-15 17 -4.2779586600240698e-15 18 -4.2779586600240698e-15
		 19 -4.2779586600240698e-15 20 -4.2779586600240698e-15 21 -4.2779586600240698e-15
		 22 -4.2779586600240698e-15 23 -4.2779586600240698e-15 24 -4.2779586600240698e-15
		 25 -4.2779586600240698e-15 26 -4.2779586600240698e-15 27 -4.2779586600240698e-15
		 28 -4.2779586600240698e-15 29 -4.2779586600240698e-15 30 -4.2779586600240698e-15
		 31 -4.2779586600240698e-15 32 -4.2779586600240698e-15 33 -4.2779586600240698e-15
		 34 -4.2779586600240698e-15 35 -4.2779586600240698e-15 36 -4.2779586600240698e-15
		 37 -4.2779586600240698e-15 38 -4.2779586600240698e-15 39 -4.2779586600240698e-15
		 40 -4.2779586600240698e-15;
createNode animCurveTL -n "L_B_toe_jnt_translateZ";
	rename -uid "2ADD4391-466F-CE5A-2799-0A8473C37918";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.3328504158703111e-14 1 -1.3328504158703111e-14
		 2 -1.3328504158703111e-14 3 -1.3328504158703111e-14 4 -1.3328504158703111e-14 5 -1.3328504158703111e-14
		 6 -1.3328504158703111e-14 7 -1.3328504158703111e-14 8 -1.3328504158703111e-14 9 -1.3328504158703111e-14
		 10 -1.3328504158703111e-14 11 -1.3328504158703111e-14 12 -1.3328504158703111e-14
		 13 -1.3328504158703111e-14 14 -1.3328504158703111e-14 15 -1.3328504158703111e-14
		 16 -1.3328504158703111e-14 17 -1.3328504158703111e-14 18 -1.3328504158703111e-14
		 19 -1.3328504158703111e-14 20 -1.3328504158703111e-14 21 -1.3328504158703111e-14
		 22 -1.3328504158703111e-14 23 -1.3328504158703111e-14 24 -1.3328504158703111e-14
		 25 -1.3328504158703111e-14 26 -1.3328504158703111e-14 27 -1.3328504158703111e-14
		 28 -1.3328504158703111e-14 29 -1.3328504158703111e-14 30 -1.3328504158703111e-14
		 31 -1.3328504158703111e-14 32 -1.3328504158703111e-14 33 -1.3328504158703111e-14
		 34 -1.3328504158703111e-14 35 -1.3328504158703111e-14 36 -1.3328504158703111e-14
		 37 -1.3328504158703111e-14 38 -1.3328504158703111e-14 39 -1.3328504158703111e-14
		 40 -1.3328504158703111e-14;
createNode animCurveTA -n "L_B_toe_jnt_rotateX";
	rename -uid "8D698490-44E7-7657-6331-56951D606619";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_B_toe_jnt_rotateY";
	rename -uid "10C7382A-46CD-13C3-6DEB-9DBB47BB8B6F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_B_toe_jnt_rotateZ";
	rename -uid "92B7F2F4-4927-6E54-5AEA-B6B8833769EF";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "L_B_toe_jnt_scaleX";
	rename -uid "9C404830-407A-78BF-977A-058FD0D8BCC9";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_B_toe_jnt_scaleY";
	rename -uid "4A9D8477-4AA8-18C4-EA8F-CDA1F62C8A01";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_B_toe_jnt_scaleZ";
	rename -uid "DDAD9754-4A1D-8FC0-4700-048720BBFCDD";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_F_ankle_jnt_visibility";
	rename -uid "7A415EDB-4AE0-EC00-BAF9-238382A63432";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "L_F_ankle_jnt_translateX";
	rename -uid "1807A632-4FA5-0335-E7D7-F9AE5BF4475B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 28.109114393380338 1 28.109114393380338
		 2 28.109114393380338 3 28.109114393380338 4 28.109114393380338 5 28.109114393380338
		 6 28.109114393380338 7 28.109114393380338 8 28.109114393380338 9 28.109114393380338
		 10 28.109114393380338 11 28.109114393380338 12 28.109114393380338 13 28.109114393380338
		 14 28.109114393380338 15 28.109114393380338 16 28.109114393380338 17 28.109114393380338
		 18 28.109114393380338 19 28.109114393380338 20 28.109114393380338 21 28.109114393380338
		 22 28.109114393380338 23 28.109114393380338 24 28.109114393380338 25 28.109114393380338
		 26 28.109114393380338 27 28.109114393380338 28 28.109114393380338 29 28.109114393380338
		 30 28.109114393380338 31 28.109114393380338 32 28.109114393380338 33 28.109114393380338
		 34 28.109114393380338 35 28.109114393380338 36 28.109114393380338 37 28.109114393380338
		 38 28.109114393380338 39 28.109114393380338 40 28.109114393380338;
createNode animCurveTL -n "L_F_ankle_jnt_translateY";
	rename -uid "8EB6E47D-48B4-3140-A84D-8F899EB0D49F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -5.9770331361984993e-15 1 -5.9770331361984993e-15
		 2 -5.9770331361984993e-15 3 -5.9770331361984993e-15 4 -5.9770331361984993e-15 5 -5.9770331361984993e-15
		 6 -5.9770331361984993e-15 7 -5.9770331361984993e-15 8 -5.9770331361984993e-15 9 -5.9770331361984993e-15
		 10 -5.9770331361984993e-15 11 -5.9770331361984993e-15 12 -5.9770331361984993e-15
		 13 -5.9770331361984993e-15 14 -5.9770331361984993e-15 15 -5.9770331361984993e-15
		 16 -5.9770331361984993e-15 17 -5.9770331361984993e-15 18 -5.9770331361984993e-15
		 19 -5.9770331361984993e-15 20 -5.9770331361984993e-15 21 -5.9770331361984993e-15
		 22 -5.9770331361984993e-15 23 -5.9770331361984993e-15 24 -5.9770331361984993e-15
		 25 -5.9770331361984993e-15 26 -5.9770331361984993e-15 27 -5.9770331361984993e-15
		 28 -5.9770331361984993e-15 29 -5.9770331361984993e-15 30 -5.9770331361984993e-15
		 31 -5.9770331361984993e-15 32 -5.9770331361984993e-15 33 -5.9770331361984993e-15
		 34 -5.9770331361984993e-15 35 -5.9770331361984993e-15 36 -5.9770331361984993e-15
		 37 -5.9770331361984993e-15 38 -5.9770331361984993e-15 39 -5.9770331361984993e-15
		 40 -5.9770331361984993e-15;
createNode animCurveTL -n "L_F_ankle_jnt_translateZ";
	rename -uid "BD66DB91-40C4-0526-CC1E-F4BDD83598F9";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.3928382854745877e-15 1 1.3928382854745877e-15
		 2 1.3928382854745877e-15 3 1.3928382854745877e-15 4 1.3928382854745877e-15 5 1.3928382854745877e-15
		 6 1.3928382854745877e-15 7 1.3928382854745877e-15 8 1.3928382854745877e-15 9 1.3928382854745877e-15
		 10 1.3928382854745877e-15 11 1.3928382854745877e-15 12 1.3928382854745877e-15 13 1.3928382854745877e-15
		 14 1.3928382854745877e-15 15 1.3928382854745877e-15 16 1.3928382854745877e-15 17 1.3928382854745877e-15
		 18 1.3928382854745877e-15 19 1.3928382854745877e-15 20 1.3928382854745877e-15 21 1.3928382854745877e-15
		 22 1.3928382854745877e-15 23 1.3928382854745877e-15 24 1.3928382854745877e-15 25 1.3928382854745877e-15
		 26 1.3928382854745877e-15 27 1.3928382854745877e-15 28 1.3928382854745877e-15 29 1.3928382854745877e-15
		 30 1.3928382854745877e-15 31 1.3928382854745877e-15 32 1.3928382854745877e-15 33 1.3928382854745877e-15
		 34 1.3928382854745877e-15 35 1.3928382854745877e-15 36 1.3928382854745877e-15 37 1.3928382854745877e-15
		 38 1.3928382854745877e-15 39 1.3928382854745877e-15 40 1.3928382854745877e-15;
createNode animCurveTA -n "L_F_ankle_jnt_rotateX";
	rename -uid "FAAA9AD6-4B50-4B7E-E9A4-56861B480F19";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.798018046431165 1 1.6674978608134521
		 2 1.4327275381235665 3 1.2023510426960093 4 0.98571604739466301 5 0.74078145199042555
		 6 0.40482851180187829 7 0.036125858254798235 8 -0.34968216578872796 9 -0.47945515992371018
		 10 -0.2933317804048291 11 0.0065717349114786452 12 0.31809904680651413 13 0.71912998805079753
		 14 1.2326917452913695 15 1.7435970801780798 16 2.3763954816325121 17 3.3410265062520503
		 18 4.4298562883386543 19 4.7998034894626338 20 4.5733990494690619 21 4.3753292457053226
		 22 4.2943838893222992 23 4.2728129770796768 24 4.2697348121964493 25 3.9239245880117295
		 26 2.9454061993833558 27 1.5628453274850076 28 0.099047378551102655 29 -1.2097824286766974
		 30 -2.1488017002628879 31 -2.5024961709033366 32 -1.3221786332052361 33 1.1398879387572283
		 34 3.0204243984693759 35 3.4871639979234934 36 3.5842282759097825 37 3.0471535566563945
		 38 2.3627469471884806 39 1.9399407432527762 40 1.798018046431165;
createNode animCurveTA -n "L_F_ankle_jnt_rotateY";
	rename -uid "BAE8AC82-46A8-1D68-C3A2-9495C6280F1B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.19869730611302902 1 -0.17779996487879113
		 2 -0.94557334742479848 3 -1.6640660274430856 4 -2.1525420252393164 5 -2.4522445459880999
		 6 -2.4546625329799499 7 -2.0792789152771709 8 -1.3342086494150094 9 -0.49510376647989418
		 10 0.10176094647412733 11 0.39461361012659368 12 0.31789157713982913 13 0.09950983123154919
		 14 -0.049291895932842125 15 0.036193975172283385 16 0.4849259606024397 17 1.2010433814762151
		 18 1.8363817001985332 19 1.8928768981353903 20 1.50761498422854 21 1.0523737068961843
		 22 0.70490189561064553 23 0.58135037935468603 24 0.65981941134342814 25 0.67552792820878438
		 26 0.43284593048802272 27 -0.018016821067378969 28 -0.49372282531812511 29 -0.87045240959028125
		 30 -0.99385255013028995 31 -0.7717942846229241 32 -0.021281040124060565 33 1.106694190589157
		 34 1.9596984989712956 35 2.2034124376601611 36 2.2027836583460472 37 1.7581305785863899
		 38 1.0438723610156724 39 0.44224971653889028 40 0.19869730611302902;
createNode animCurveTA -n "L_F_ankle_jnt_rotateZ";
	rename -uid "257695A2-4F75-7FD1-FFA2-4E91C73B7D0B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -0.046337155423228971 1 -1.1562753174980507
		 2 -3.3106668860295976 3 -5.2023624570083866 4 -6.4090213125926931 5 -7.0919312006118957
		 6 -6.9857166849652934 7 -5.911981604293068 8 -3.9472792329836919 9 -1.6912506322236533
		 10 -0.021616104139821821 11 0.796905631169465 12 0.52213307879618664 13 -0.18730280771443059
		 14 -0.70694376423422511 15 -0.5313316348749928 16 0.89794195842144686 17 3.5832823379976722
		 18 6.4681232180920487 19 6.72756216079285 20 4.8087780324976226 21 2.8193656524080724
		 22 1.4594870157645878 23 1.0218009609388052 24 1.3580954047138454 25 1.5469921208078579
		 26 0.85904260494519091 27 -0.51601652814222287 28 -1.9018924366179304 29 -2.8793248062534573
		 30 -3.1234535555906549 31 -2.4410388458295849 32 -0.23005456941906718 33 3.5180944359070212
		 34 6.9198430877407953 35 8.0196662109756645 36 7.9790569616935425 37 5.8142316415125679
		 38 2.8776502203168648 39 0.74899023753198646 40 -0.04633715542322895;
createNode animCurveTU -n "L_F_ankle_jnt_scaleX";
	rename -uid "B7924970-440C-DECB-1DF2-3BA4C4A86936";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_F_ankle_jnt_scaleY";
	rename -uid "9EA20170-4131-E7DD-C0A8-0EA959398ABA";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_F_ankle_jnt_scaleZ";
	rename -uid "95310EBA-490E-A385-0E6C-1DB1219D51D5";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_F_elbow_jnt_visibility";
	rename -uid "3939D215-489D-6A3D-46D1-EDA23239FF56";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "L_F_elbow_jnt_translateX";
	rename -uid "D8BA410F-4C33-1ED2-8EE7-B881D2B39108";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 51.388393366150176 1 51.388393366150176
		 2 51.388393366150176 3 51.388393366150176 4 51.388393366150176 5 51.388393366150176
		 6 51.388393366150176 7 51.388393366150176 8 51.388393366150176 9 51.388393366150176
		 10 51.388393366150176 11 51.388393366150176 12 51.388393366150176 13 51.388393366150176
		 14 51.388393366150176 15 51.388393366150176 16 51.388393366150176 17 51.388393366150176
		 18 51.388393366150176 19 51.388393366150176 20 51.388393366150176 21 51.388393366150176
		 22 51.388393366150176 23 51.388393366150176 24 51.388393366150176 25 51.388393366150176
		 26 51.388393366150176 27 51.388393366150176 28 51.388393366150176 29 51.388393366150176
		 30 51.388393366150176 31 51.388393366150176 32 51.388393366150176 33 51.388393366150176
		 34 51.388393366150176 35 51.388393366150176 36 51.388393366150176 37 51.388393366150176
		 38 51.388393366150176 39 51.388393366150176 40 51.388393366150176;
createNode animCurveTL -n "L_F_elbow_jnt_translateY";
	rename -uid "CFF02119-4505-6DCC-D486-88891B0B4155";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.8380138906937199e-14 1 -1.8380138906937199e-14
		 2 -1.8380138906937199e-14 3 -1.8380138906937199e-14 4 -1.8380138906937199e-14 5 -1.8380138906937199e-14
		 6 -1.8380138906937199e-14 7 -1.8380138906937199e-14 8 -1.8380138906937199e-14 9 -1.8380138906937199e-14
		 10 -1.8380138906937199e-14 11 -1.8380138906937199e-14 12 -1.8380138906937199e-14
		 13 -1.8380138906937199e-14 14 -1.8380138906937199e-14 15 -1.8380138906937199e-14
		 16 -1.8380138906937199e-14 17 -1.8380138906937199e-14 18 -1.8380138906937199e-14
		 19 -1.8380138906937199e-14 20 -1.8380138906937199e-14 21 -1.8380138906937199e-14
		 22 -1.8380138906937199e-14 23 -1.8380138906937199e-14 24 -1.8380138906937199e-14
		 25 -1.8380138906937199e-14 26 -1.8380138906937199e-14 27 -1.8380138906937199e-14
		 28 -1.8380138906937199e-14 29 -1.8380138906937199e-14 30 -1.8380138906937199e-14
		 31 -1.8380138906937199e-14 32 -1.8380138906937199e-14 33 -1.8380138906937199e-14
		 34 -1.8380138906937199e-14 35 -1.8380138906937199e-14 36 -1.8380138906937199e-14
		 37 -1.8380138906937199e-14 38 -1.8380138906937199e-14 39 -1.8380138906937199e-14
		 40 -1.8380138906937199e-14;
createNode animCurveTL -n "L_F_elbow_jnt_translateZ";
	rename -uid "ECC03549-409E-1616-4C86-8483B2218839";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.251293237059672e-14 1 1.251293237059672e-14
		 2 1.251293237059672e-14 3 1.251293237059672e-14 4 1.251293237059672e-14 5 1.251293237059672e-14
		 6 1.251293237059672e-14 7 1.251293237059672e-14 8 1.251293237059672e-14 9 1.251293237059672e-14
		 10 1.251293237059672e-14 11 1.251293237059672e-14 12 1.251293237059672e-14 13 1.251293237059672e-14
		 14 1.251293237059672e-14 15 1.251293237059672e-14 16 1.251293237059672e-14 17 1.251293237059672e-14
		 18 1.251293237059672e-14 19 1.251293237059672e-14 20 1.251293237059672e-14 21 1.251293237059672e-14
		 22 1.251293237059672e-14 23 1.251293237059672e-14 24 1.251293237059672e-14 25 1.251293237059672e-14
		 26 1.251293237059672e-14 27 1.251293237059672e-14 28 1.251293237059672e-14 29 1.251293237059672e-14
		 30 1.251293237059672e-14 31 1.251293237059672e-14 32 1.251293237059672e-14 33 1.251293237059672e-14
		 34 1.251293237059672e-14 35 1.251293237059672e-14 36 1.251293237059672e-14 37 1.251293237059672e-14
		 38 1.251293237059672e-14 39 1.251293237059672e-14 40 1.251293237059672e-14;
createNode animCurveTA -n "L_F_elbow_jnt_rotateX";
	rename -uid "8D90A078-467C-4D63-3E1D-9DA95FDE4DE4";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -4.4745688769956515e-29 1 7.85479307712905e-17
		 2 2.3644545997738504e-16 3 3.8534352919736383e-16 4 4.9183334581355071e-16 5 5.6449677829274929e-16
		 6 5.9094936861753229e-16 7 5.5624557167761993e-16 8 4.2426058881812362e-16 9 2.5748194498328975e-16
		 10 1.4258778101729027e-16 11 7.2528376251691627e-17 12 6.9422095166576701e-17 13 8.7637320262670955e-17
		 14 8.4882004931414875e-17 15 2.8454714296283256e-17 16 -1.2865106743662788e-16 17 -3.8189587596665325e-16
		 18 -6.276944124998049e-16 19 -6.5320424753219782e-16 20 -5.0204632463624323e-16 21 -3.3486382171097189e-16
		 22 -2.1736252529095486e-16 23 -1.8757442125357147e-16 24 -2.3369617089605239e-16
		 25 -2.7266302489068653e-16 26 -2.3771420049292768e-16 27 -1.3670882970565551e-16
		 28 -2.3235646539950375e-17 29 7.2845625716071491e-17 30 1.1352252081188431e-16 31 7.1698898613515342e-17
		 32 -9.3014911035303515e-17 33 -3.5885897806946677e-16 34 -5.9275940460659793e-16
		 35 -6.6201760157659857e-16 36 -6.5752080048390477e-16 37 -4.9282029768543737e-16
		 38 -2.5473427918027577e-16 39 -7.1617911494962524e-17 40 -4.4745975096483483e-29;
createNode animCurveTA -n "L_F_elbow_jnt_rotateY";
	rename -uid "B0E128C7-4BA5-FE23-E8AC-1A85E6C55581";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -3.8821079604618444e-13 1 0.64929312388079319
		 2 1.9545061191129465 3 3.1853277531692172 4 4.0655941715621102 5 4.6662448392887299
		 6 4.884907315028892 7 4.5980386922099159 8 3.5070240596829207 9 2.1283979699972946
		 10 1.1786595121584742 11 0.5995342655630429 12 0.57385711621484814 13 0.72442786058330033
		 14 0.70165186532612434 15 0.23521243848376525 16 -1.1161686114282876 17 -3.3133047247964735
		 18 -5.4458374482303924 19 -5.6671591808300574 20 -4.3557224996826562 21 -2.9052575648532213
		 22 -1.8858236691278114 23 -1.6273839423306322 24 -2.0275333564071958 25 -2.3656073435223357
		 26 -2.0623935297829914 27 -1.186077252703029 28 -0.20159101553440412 29 0.60215671396816373
		 30 0.93840017738203374 31 0.59267763520254468 32 -0.80699154823212949 33 -3.1134380400510557
		 34 -5.1427435056200475 35 -5.7436232890708254 36 -5.7046093241538731 37 -4.2756780671267158
		 38 -2.210058667534105 39 -0.62135251902284239 40 -3.8822645580614053e-13;
createNode animCurveTA -n "L_F_elbow_jnt_rotateZ";
	rename -uid "551A532B-4B92-8E5A-4FDF-C39753EB24E8";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -9.6468980171874327e-13 1 1.5383023627161334
		 2 4.6306071485928539 3 7.546664254564158 4 9.6321875127498764 5 11.05524638608013
		 6 11.573300544821238 7 10.893652687727077 8 8.3088257039713103 9 5.0425909433289657
		 10 2.7924748402604491 11 1.4204138983215748 12 1.3595797110225347 13 1.7163112445917117
		 14 1.6623504585788942 15 0.55726425639189281 16 -2.7736386710774017 17 -8.2334425280059129
		 18 -13.532709295134808 19 -14.082685804062752 20 -10.823813034969893 21 -7.2194600787112204
		 22 -4.6862036810305279 23 -4.0439902976331306 24 -5.0383471338027954 25 -5.8784487768224549
		 26 -5.1249734051076681 27 -2.9473591187739001 28 -0.50094638991190854 29 1.4266269913443173
		 30 2.2232534997629951 31 1.4041692003633042 32 -2.0053448399207805 33 -7.7367810378027873
		 34 -12.779531798844516 35 -14.272696350314051 36 -14.175748057110171 37 -10.624905512156195
		 38 -5.4919159371256221 39 -1.5440385596650867 40 -9.6470199182140302e-13;
createNode animCurveTU -n "L_F_elbow_jnt_scaleX";
	rename -uid "45B6EB10-4E6B-7A9F-8A26-0EB7ABBEB970";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_F_elbow_jnt_scaleY";
	rename -uid "2FE3D41F-4E83-B5AF-4335-708A688B22B7";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_F_elbow_jnt_scaleZ";
	rename -uid "868ADFA3-4E77-17B6-043A-B19A8C3F6197";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_F_shoulder_jnt_visibility";
	rename -uid "45C6B6C7-4443-6EB8-24A9-5397D0BBE13A";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "L_F_shoulder_jnt_translateX";
	rename -uid "760558DE-454C-2B49-F953-B7B459D1864D";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 85.078812727200471 1 85.078812727200471
		 2 85.078812727200471 3 85.078812727200471 4 85.078812727200471 5 85.078812727200471
		 6 85.078812727200471 7 85.078812727200471 8 85.078812727200471 9 85.078812727200471
		 10 85.078812727200471 11 85.078812727200471 12 85.078812727200471 13 85.078812727200471
		 14 85.078812727200471 15 85.078812727200471 16 85.078812727200471 17 85.078812727200471
		 18 85.078812727200471 19 85.078812727200471 20 85.078812727200471 21 85.078812727200471
		 22 85.078812727200471 23 85.078812727200471 24 85.078812727200471 25 85.078812727200471
		 26 85.078812727200471 27 85.078812727200471 28 85.078812727200471 29 85.078812727200471
		 30 85.078812727200471 31 85.078812727200471 32 85.078812727200471 33 85.078812727200471
		 34 85.078812727200471 35 85.078812727200471 36 85.078812727200471 37 85.078812727200471
		 38 85.078812727200471 39 85.078812727200471 40 85.078812727200471;
createNode animCurveTL -n "L_F_shoulder_jnt_translateY";
	rename -uid "1B71698E-40D6-57E8-FF04-72B798BBFF01";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -8.0598282011885772e-14 1 -8.0598282011885772e-14
		 2 -8.0598282011885772e-14 3 -8.0598282011885772e-14 4 -8.0598282011885772e-14 5 -8.0598282011885772e-14
		 6 -8.0598282011885772e-14 7 -8.0598282011885772e-14 8 -8.0598282011885772e-14 9 -8.0598282011885772e-14
		 10 -8.0598282011885772e-14 11 -8.0598282011885772e-14 12 -8.0598282011885772e-14
		 13 -8.0598282011885772e-14 14 -8.0598282011885772e-14 15 -8.0598282011885772e-14
		 16 -8.0598282011885772e-14 17 -8.0598282011885772e-14 18 -8.0598282011885772e-14
		 19 -8.0598282011885772e-14 20 -8.0598282011885772e-14 21 -8.0598282011885772e-14
		 22 -8.0598282011885772e-14 23 -8.0598282011885772e-14 24 -8.0598282011885772e-14
		 25 -8.0598282011885772e-14 26 -8.0598282011885772e-14 27 -8.0598282011885772e-14
		 28 -8.0598282011885772e-14 29 -8.0598282011885772e-14 30 -8.0598282011885772e-14
		 31 -8.0598282011885772e-14 32 -8.0598282011885772e-14 33 -8.0598282011885772e-14
		 34 -8.0598282011885772e-14 35 -8.0598282011885772e-14 36 -8.0598282011885772e-14
		 37 -8.0598282011885772e-14 38 -8.0598282011885772e-14 39 -8.0598282011885772e-14
		 40 -8.0598282011885772e-14;
createNode animCurveTL -n "L_F_shoulder_jnt_translateZ";
	rename -uid "E836DAAC-413D-A0CC-160A-2793D0EED846";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.022860331953822e-13 1 -1.022860331953822e-13
		 2 -1.022860331953822e-13 3 -1.022860331953822e-13 4 -1.022860331953822e-13 5 -1.022860331953822e-13
		 6 -1.022860331953822e-13 7 -1.022860331953822e-13 8 -1.022860331953822e-13 9 -1.022860331953822e-13
		 10 -1.022860331953822e-13 11 -1.022860331953822e-13 12 -1.022860331953822e-13 13 -1.022860331953822e-13
		 14 -1.022860331953822e-13 15 -1.022860331953822e-13 16 -1.022860331953822e-13 17 -1.022860331953822e-13
		 18 -1.022860331953822e-13 19 -1.022860331953822e-13 20 -1.022860331953822e-13 21 -1.022860331953822e-13
		 22 -1.022860331953822e-13 23 -1.022860331953822e-13 24 -1.022860331953822e-13 25 -1.022860331953822e-13
		 26 -1.022860331953822e-13 27 -1.022860331953822e-13 28 -1.022860331953822e-13 29 -1.022860331953822e-13
		 30 -1.022860331953822e-13 31 -1.022860331953822e-13 32 -1.022860331953822e-13 33 -1.022860331953822e-13
		 34 -1.022860331953822e-13 35 -1.022860331953822e-13 36 -1.022860331953822e-13 37 -1.022860331953822e-13
		 38 -1.022860331953822e-13 39 -1.022860331953822e-13 40 -1.022860331953822e-13;
createNode animCurveTA -n "L_F_shoulder_jnt_rotateX";
	rename -uid "5A0D3EB0-42F7-7465-518C-70B16786311B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.26100436288313555 1 0.32737284926242288
		 2 0.43014391284539222 3 0.49766514551959895 4 0.521243293847233 5 0.53505628029355468
		 6 0.48414002375345255 7 0.30029785038093271 8 0.074695505415058885 9 -0.10792522677032589
		 10 -0.1959836447486174 11 -0.19194753394048877 12 -0.13164414492718413 13 -0.038749934733564834
		 14 0.08604586303272943 15 0.31195158798010969 16 0.8759674767850224 17 1.7023471926428997
		 18 2.3854737804192663 19 2.7804362616587968 20 3.1026052503928527 21 3.4124083119024813
		 22 3.7329084624604518 23 4.0389717254268591 24 4.2947474906834691 25 4.6057375026135343
		 26 4.8890414514360607 27 4.9850228485735055 28 4.9169862185006883 29 4.7358977160202542
		 30 4.4874831802579038 31 4.2606291400512397 32 3.6469005163778898 33 2.6184257349029481
		 34 1.946229559499417 35 1.7742708812202284 36 1.7154829323355678 37 1.4655138058628829
		 38 0.96304880392542902 39 0.47879838422190318 40 0.26100436288313555;
createNode animCurveTA -n "L_F_shoulder_jnt_rotateY";
	rename -uid "293DDE82-4936-B90C-9079-3081399B1FAC";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.032264149769202187 1 -0.16087548858697445
		 2 -0.66865016870967187 3 -1.3114490379198915 4 -1.9013872591427881 5 -2.3236851757410442
		 6 -2.6073446863724827 7 -2.8376257736766783 8 -2.2762144776184607 9 -1.3771360271909414
		 10 -0.84567329783195522 11 -0.42444504387622639 12 -0.27852198780963855 13 -0.2614833384710934
		 14 -0.21430596799240917 15 0.087258287802627357 16 1.0039411524375752 17 2.4170560123622398
		 18 3.669311743474581 19 3.4523142774447662 20 1.979687264146972 21 0.30894047098432992
		 22 -0.94882537307259152 23 -1.3434448691783478 24 -0.92973971570170366 25 -0.17198344728452447
		 26 0.56738970001058253 27 1.1465433914680747 28 1.6726307023649538 29 2.1492559940276927
		 30 2.4905688721716253 31 2.6870815497591622 32 2.5222296981208774 33 2.3314894529848513
		 34 2.5520240450683382 35 2.6187429369615827 36 2.642348087547179 37 2.023842666950062
		 38 1.0540681011455901 39 0.31944737253599659 40 0.03226414976920218;
createNode animCurveTA -n "L_F_shoulder_jnt_rotateZ";
	rename -uid "0634ADB2-48DF-1D5F-A4A4-5B839B802997";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -0.032956412307980375 1 -0.25752278921674554
		 2 -0.88127468685964616 3 -1.742357039334812 4 -2.4637672359475875 5 -3.1018658321391395
		 6 -3.9371154355848756 7 -4.7968472634006201 8 -4.8851345067806431 9 -4.3809222369923164
		 10 -3.8429827537310377 11 -3.1740267405276086 12 -2.5804350737643458 13 -1.9181510896337326
		 14 -1.096691268938079 15 -0.093258728533660448 16 1.6365700696634689 17 4.0936284596197217
		 18 6.2491555227640658 19 6.595473762509191 20 5.4942810436189484 21 4.0735298624894778
		 22 2.956287617577666 23 2.6115325795968691 24 3.018406353935319 25 3.3839041525513962
		 26 3.0159821765131434 27 1.9296089347724028 28 0.57201269354321926 29 -0.69267336945917712
		 30 -1.6124638071381614 31 -1.860583854784043 32 -0.64174056843118366 33 1.6721115843767445
		 34 3.4016096522528443 35 3.7729229890583227 36 3.9325256805160307 37 3.1063309033347055
		 38 1.6464002675046141 39 0.45036667637846417 40 -0.032956412307980368;
createNode animCurveTU -n "L_F_shoulder_jnt_scaleX";
	rename -uid "9585977E-4487-5637-1662-34A3074A9069";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_F_shoulder_jnt_scaleY";
	rename -uid "7A000659-4EC4-0510-CD8D-B29CDBDCB41B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_F_shoulder_jnt_scaleZ";
	rename -uid "571ABA33-4E55-1950-D130-2588EE744461";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_F_toe_jnt_visibility";
	rename -uid "77760CFD-47B6-9333-5309-41B28F517F27";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "L_F_toe_jnt_translateX";
	rename -uid "7A964433-450E-FA66-EDAE-8181ED48F50F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 25.2327435189653 1 25.2327435189653 2 25.2327435189653
		 3 25.2327435189653 4 25.2327435189653 5 25.2327435189653 6 25.2327435189653 7 25.2327435189653
		 8 25.2327435189653 9 25.2327435189653 10 25.2327435189653 11 25.2327435189653 12 25.2327435189653
		 13 25.2327435189653 14 25.2327435189653 15 25.2327435189653 16 25.2327435189653 17 25.2327435189653
		 18 25.2327435189653 19 25.2327435189653 20 25.2327435189653 21 25.2327435189653 22 25.2327435189653
		 23 25.2327435189653 24 25.2327435189653 25 25.2327435189653 26 25.2327435189653 27 25.2327435189653
		 28 25.2327435189653 29 25.2327435189653 30 25.2327435189653 31 25.2327435189653 32 25.2327435189653
		 33 25.2327435189653 34 25.2327435189653 35 25.2327435189653 36 25.2327435189653 37 25.2327435189653
		 38 25.2327435189653 39 25.2327435189653 40 25.2327435189653;
createNode animCurveTL -n "L_F_toe_jnt_translateY";
	rename -uid "73F2A8FA-40EF-8E2D-BF11-92A935415614";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -9.4990880393351376e-15 1 -9.4990880393351376e-15
		 2 -9.4990880393351376e-15 3 -9.4990880393351376e-15 4 -9.4990880393351376e-15 5 -9.4990880393351376e-15
		 6 -9.4990880393351376e-15 7 -9.4990880393351376e-15 8 -9.4990880393351376e-15 9 -9.4990880393351376e-15
		 10 -9.4990880393351376e-15 11 -9.4990880393351376e-15 12 -9.4990880393351376e-15
		 13 -9.4990880393351376e-15 14 -9.4990880393351376e-15 15 -9.4990880393351376e-15
		 16 -9.4990880393351376e-15 17 -9.4990880393351376e-15 18 -9.4990880393351376e-15
		 19 -9.4990880393351376e-15 20 -9.4990880393351376e-15 21 -9.4990880393351376e-15
		 22 -9.4990880393351376e-15 23 -9.4990880393351376e-15 24 -9.4990880393351376e-15
		 25 -9.4990880393351376e-15 26 -9.4990880393351376e-15 27 -9.4990880393351376e-15
		 28 -9.4990880393351376e-15 29 -9.4990880393351376e-15 30 -9.4990880393351376e-15
		 31 -9.4990880393351376e-15 32 -9.4990880393351376e-15 33 -9.4990880393351376e-15
		 34 -9.4990880393351376e-15 35 -9.4990880393351376e-15 36 -9.4990880393351376e-15
		 37 -9.4990880393351376e-15 38 -9.4990880393351376e-15 39 -9.4990880393351376e-15
		 40 -9.4990880393351376e-15;
createNode animCurveTL -n "L_F_toe_jnt_translateZ";
	rename -uid "F6CA2F86-443E-F573-D17D-928895E350E4";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.5611553356662841e-14 1 1.5611553356662841e-14
		 2 1.5611553356662841e-14 3 1.5611553356662841e-14 4 1.5611553356662841e-14 5 1.5611553356662841e-14
		 6 1.5611553356662841e-14 7 1.5611553356662841e-14 8 1.5611553356662841e-14 9 1.5611553356662841e-14
		 10 1.5611553356662841e-14 11 1.5611553356662841e-14 12 1.5611553356662841e-14 13 1.5611553356662841e-14
		 14 1.5611553356662841e-14 15 1.5611553356662841e-14 16 1.5611553356662841e-14 17 1.5611553356662841e-14
		 18 1.5611553356662841e-14 19 1.5611553356662841e-14 20 1.5611553356662841e-14 21 1.5611553356662841e-14
		 22 1.5611553356662841e-14 23 1.5611553356662841e-14 24 1.5611553356662841e-14 25 1.5611553356662841e-14
		 26 1.5611553356662841e-14 27 1.5611553356662841e-14 28 1.5611553356662841e-14 29 1.5611553356662841e-14
		 30 1.5611553356662841e-14 31 1.5611553356662841e-14 32 1.5611553356662841e-14 33 1.5611553356662841e-14
		 34 1.5611553356662841e-14 35 1.5611553356662841e-14 36 1.5611553356662841e-14 37 1.5611553356662841e-14
		 38 1.5611553356662841e-14 39 1.5611553356662841e-14 40 1.5611553356662841e-14;
createNode animCurveTA -n "L_F_toe_jnt_rotateX";
	rename -uid "3C4F8ABE-4C70-0D2A-0A29-8FA61D8665A1";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_F_toe_jnt_rotateY";
	rename -uid "BBE0697D-459F-B52C-83A3-DF93722EA6ED";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_F_toe_jnt_rotateZ";
	rename -uid "7ADC98D1-45A3-5674-0B53-2BA0BB42FE05";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "L_F_toe_jnt_scaleX";
	rename -uid "CA90A280-4F96-E100-7619-01816EB46510";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_F_toe_jnt_scaleY";
	rename -uid "A470BE69-430F-6EA2-169D-AB898C5911C2";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_F_toe_jnt_scaleZ";
	rename -uid "051D0D1F-4D15-F360-D65A-81ACDEB9E1B2";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_brow_jnt_visibility";
	rename -uid "110B8375-4306-C978-FE2D-E8B38A053FE5";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "L_brow_jnt_translateX";
	rename -uid "DD75EB87-40D9-CF91-F03D-758D51A76B44";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 17.67761992495732 1 17.677620419819018
		 2 17.677619924957284 3 17.67761992495727 4 17.677619924957273 5 17.677619924957295
		 6 17.677619924957305 7 17.677619924957312 8 17.677619924957273 9 17.677619924957266
		 10 17.677619924957273 11 17.677619924957298 12 17.677619924957288 13 17.677619924957277
		 14 17.677619924957281 15 17.677619924957291 16 17.677619924957291 17 17.677620287413728
		 18 17.677619924957266 19 17.677619924957291 20 17.677619924957277 21 17.677619924957291
		 22 17.677619924957249 23 17.677619924957273 24 17.677619924957291 25 17.677619924957284
		 26 17.677619924957263 27 17.677619924957291 28 17.677619924957291 29 17.677619924957312
		 30 17.677619924957277 31 17.677619924957291 32 17.677619924957291 33 17.677619924957288
		 34 17.677619924957273 35 17.677619924957277 36 17.677619924957277 37 17.677619924957291
		 38 17.677619924957312 39 17.677619924957298 40 17.67761992495732;
createNode animCurveTL -n "L_brow_jnt_translateY";
	rename -uid "80974FEA-4158-8084-D029-7AB073D112EE";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 40.936095313811364 1 40.936095100149487
		 2 40.93609531381135 3 40.936095313811421 4 40.93609531381135 5 40.936095313811407
		 6 40.936095313811407 7 40.936095313811379 8 40.936095313811379 9 40.936095313811379
		 10 40.936095313811364 11 40.936095313811421 12 40.93609531381135 13 40.936095313811421
		 14 40.936095313811379 15 40.936095313811393 16 40.936095313811393 17 40.93609515726726
		 18 40.936095313811379 19 40.936095313811336 20 40.936095313811407 21 40.936095313811379
		 22 40.936095313811393 23 40.936095313811393 24 40.936095313811364 25 40.936095313811364
		 26 40.936095313811393 27 40.936095313811364 28 40.936095313811379 29 40.936095313811393
		 30 40.936095313811379 31 40.936095313811393 32 40.936095313811364 33 40.936095313811407
		 34 40.936095313811379 35 40.936095313811393 36 40.936095313811379 37 40.936095313811407
		 38 40.936095313811364 39 40.936095313811379 40 40.936095313811421;
createNode animCurveTL -n "L_brow_jnt_translateZ";
	rename -uid "D8601F62-4238-4332-045F-38A59F25184D";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -16.754699168376511 1 -16.754699168287225
		 2 -16.754699168376519 3 -16.754699168376522 4 -16.754699168376515 5 -16.754699168376526
		 6 -16.754699168376522 7 -16.754699168376526 8 -16.754699168376511 9 -16.754699168376519
		 10 -16.754699168376511 11 -16.754699168376522 12 -16.754699168376526 13 -16.754699168376519
		 14 -16.754699168376519 15 -16.754699168376526 16 -16.754699168376526 17 -16.754699168432484
		 18 -16.754699168376511 19 -16.754699168376511 20 -16.754699168376519 21 -16.754699168376519
		 22 -16.754699168376519 23 -16.754699168376519 24 -16.754699168376519 25 -16.754699168376526
		 26 -16.754699168376519 27 -16.754699168376519 28 -16.754699168376522 29 -16.754699168376519
		 30 -16.754699168376519 31 -16.754699168376522 32 -16.754699168376522 33 -16.754699168376519
		 34 -16.754699168376515 35 -16.754699168376522 36 -16.754699168376522 37 -16.754699168376515
		 38 -16.754699168376522 39 -16.754699168376522 40 -16.754699168376511;
createNode animCurveTA -n "L_brow_jnt_rotateX";
	rename -uid "85E9A616-441B-E4CC-D472-DDA91CD661E8";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_brow_jnt_rotateY";
	rename -uid "470EB9F9-4A8E-EE6D-5634-C686768A0DD3";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_brow_jnt_rotateZ";
	rename -uid "50289D6D-4F01-B4C6-B98B-98A33B06AAF3";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "L_brow_jnt_scaleX";
	rename -uid "6EDCEF1F-482A-E8EE-6E66-3192CC341B56";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_brow_jnt_scaleY";
	rename -uid "93E3961D-49EF-E7E2-EB06-77BE6993A489";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_brow_jnt_scaleZ";
	rename -uid "46F1A73F-47C1-9702-39CE-BC8020B0EC39";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_cheek_jnt_visibility";
	rename -uid "60CA23E2-4C62-CCB3-ABF2-5C8EFE8C3E08";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "L_cheek_jnt_translateX";
	rename -uid "6E480A6D-4C68-8B17-C1C0-EB80129C1FF0";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 22.738426919838908 1 22.738427046018497
		 2 22.738426919838886 3 22.738426919838886 4 22.738426919838876 5 22.73842691983889
		 6 22.738426919838915 7 22.738426919838922 8 22.738426919838904 9 22.738426919838876
		 10 22.738426919838876 11 22.738426919838901 12 22.738426919838904 13 22.738426919838908
		 14 22.738426919838897 15 22.738426919838922 16 22.738426919838908 17 22.738427012326543
		 18 22.738426919838869 19 22.738426919838915 20 22.738426919838901 21 22.738426919838901
		 22 22.738426919838879 23 22.73842691983889 24 22.738426919838894 25 22.738426919838908
		 26 22.738426919838886 27 22.738426919838915 28 22.738426919838922 29 22.738426919838922
		 30 22.738426919838894 31 22.738426919838901 32 22.738426919838915 33 22.738426919838918
		 34 22.73842691983889 35 22.738426919838894 36 22.738426919838894 37 22.738426919838922
		 38 22.738426919838894 39 22.738426919838901 40 22.738426919838922;
createNode animCurveTL -n "L_cheek_jnt_translateY";
	rename -uid "D18B9B68-4F11-630F-AED7-48AA7B7214CA";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 10.442046047733555 1 10.44204577296378
		 2 10.442046047733513 3 10.442046047733555 4 10.442046047733513 5 10.442046047733541
		 6 10.442046047733513 7 10.442046047733513 8 10.442046047733484 9 10.442046047733484
		 10 10.442046047733498 11 10.442046047733527 12 10.442046047733513 13 10.442046047733498
		 14 10.442046047733513 15 10.442046047733498 16 10.442046047733498 17 10.442045846335873
		 18 10.442046047733541 19 10.442046047733498 20 10.442046047733513 21 10.442046047733541
		 22 10.442046047733527 23 10.442046047733527 24 10.442046047733498 25 10.442046047733527
		 26 10.442046047733527 27 10.442046047733527 28 10.442046047733541 29 10.442046047733527
		 30 10.442046047733513 31 10.442046047733527 32 10.442046047733498 33 10.442046047733513
		 34 10.442046047733513 35 10.442046047733527 36 10.442046047733484 37 10.442046047733513
		 38 10.442046047733527 39 10.442046047733541 40 10.442046047733555;
createNode animCurveTL -n "L_cheek_jnt_translateZ";
	rename -uid "5CB74364-488F-BB92-C421-27929EF18F2E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -43.436549431515651 1 -43.43654943151639
		 2 -43.436549431515665 3 -43.436549431515672 4 -43.436549431515658 5 -43.436549431515672
		 6 -43.436549431515665 7 -43.436549431515665 8 -43.43654943151563 9 -43.436549431515651
		 10 -43.436549431515644 11 -43.436549431515672 12 -43.436549431515651 13 -43.436549431515658
		 14 -43.436549431515651 15 -43.436549431515665 16 -43.436549431515658 17 -43.436549431515189
		 18 -43.436549431515637 19 -43.436549431515644 20 -43.436549431515658 21 -43.436549431515665
		 22 -43.436549431515651 23 -43.436549431515665 24 -43.436549431515637 25 -43.436549431515658
		 26 -43.436549431515644 27 -43.436549431515665 28 -43.436549431515658 29 -43.436549431515679
		 30 -43.436549431515665 31 -43.436549431515658 32 -43.436549431515665 33 -43.436549431515665
		 34 -43.436549431515651 35 -43.436549431515658 36 -43.436549431515658 37 -43.436549431515658
		 38 -43.436549431515679 39 -43.436549431515665 40 -43.436549431515651;
createNode animCurveTA -n "L_cheek_jnt_rotateX";
	rename -uid "5FB09289-4D94-93ED-949B-E4BAAA201842";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_cheek_jnt_rotateY";
	rename -uid "4DF014F8-4290-94D1-9759-3DA6DA46B557";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_cheek_jnt_rotateZ";
	rename -uid "C271736F-41BA-39E7-BD20-A78F81759E5B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "L_cheek_jnt_scaleX";
	rename -uid "19948F99-4086-B17D-E369-1D80D89F8E9B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 0.99999999999999978 2 1 3 1 4 0.99999999999999978
		 5 0.99999999999999989 6 0.99999999999999989 7 1.0000000000000002 8 0.99999999999999944
		 9 0.99999999999999956 10 0.99999999999999956 11 0.99999999999999978 12 0.99999999999999944
		 13 0.99999999999999967 14 0.99999999999999956 15 0.99999999999999989 16 1 17 0.99999999999999989
		 18 0.99999999999999944 19 0.99999999999999933 20 0.99999999999999989 21 0.99999999999999989
		 22 0.99999999999999944 23 1 24 0.99999999999999944 25 0.99999999999999989 26 0.99999999999999956
		 27 1 28 0.99999999999999967 29 1 30 0.99999999999999989 31 0.99999999999999967 32 0.99999999999999989
		 33 0.99999999999999978 34 0.99999999999999989 35 0.99999999999999967 36 0.99999999999999956
		 37 0.99999999999999989 38 1 39 0.99999999999999989 40 0.99999999999999989;
createNode animCurveTU -n "L_cheek_jnt_scaleY";
	rename -uid "E96FABB4-440D-A734-62A0-2F82B91325AD";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.99999999999999978 1 0.99999999999999967
		 2 0.99999999999999967 3 1 4 0.99999999999999967 5 1 6 1 7 1 8 0.99999999999999956
		 9 1 10 0.99999999999999956 11 0.99999999999999978 12 0.99999999999999989 13 0.99999999999999978
		 14 0.99999999999999956 15 1 16 1 17 0.99999999999999978 18 0.99999999999999989 19 0.99999999999999967
		 20 0.99999999999999978 21 0.99999999999999978 22 0.99999999999999978 23 0.99999999999999978
		 24 0.99999999999999978 25 0.99999999999999967 26 0.99999999999999967 27 0.99999999999999978
		 28 1.0000000000000002 29 0.99999999999999978 30 0.99999999999999978 31 0.99999999999999978
		 32 0.99999999999999978 33 0.99999999999999978 34 0.99999999999999978 35 0.99999999999999978
		 36 0.99999999999999978 37 1 38 1 39 0.99999999999999989 40 0.99999999999999978;
createNode animCurveTU -n "L_cheek_jnt_scaleZ";
	rename -uid "678CA6E6-4A6D-53A1-A58C-49B9AD7F825E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 0.99999999999999933 2 0.99999999999999978
		 3 0.99999999999999978 4 0.99999999999999922 5 1.0000000000000002 6 1 7 0.99999999999999978
		 8 0.99999999999999944 9 0.99999999999999922 10 0.99999999999999944 11 0.99999999999999978
		 12 0.99999999999999956 13 1 14 0.99999999999999944 15 1 16 0.99999999999999956 17 0.99999999999999944
		 18 0.99999999999999933 19 0.99999999999999922 20 0.99999999999999978 21 0.99999999999999978
		 22 0.99999999999999944 23 1 24 0.99999999999999944 25 0.99999999999999978 26 0.99999999999999922
		 27 0.99999999999999967 28 0.99999999999999967 29 0.99999999999999978 30 0.99999999999999989
		 31 0.99999999999999956 32 0.99999999999999967 33 0.99999999999999989 34 0.99999999999999911
		 35 0.99999999999999933 36 0.99999999999999944 37 0.99999999999999967 38 0.99999999999999978
		 39 1 40 1;
createNode animCurveTU -n "L_lipcorner_jnt_visibility";
	rename -uid "90B3A2B6-47AD-EFBE-A4B6-3C9F8D80C9E2";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "L_lipcorner_jnt_translateX";
	rename -uid "597676E1-4CC9-1F2F-A06B-8BB14FFA7DD6";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 39.373373998453346 1 39.373374044281888
		 2 39.373373998453353 3 39.373373998453332 4 39.373373998453346 5 39.373373998453346
		 6 39.373373998453367 7 39.373373998453381 8 39.373373998453346 9 39.373373998453332
		 10 39.373373998453332 11 39.373373998453332 12 39.37337399845336 13 39.37337399845336
		 14 39.373373998453339 15 39.373373998453374 16 39.373373998453367 17 39.373374032083859
		 18 39.373373998453332 19 39.373373998453353 20 39.373373998453346 21 39.373373998453346
		 22 39.373373998453332 23 39.373373998453332 24 39.373373998453332 25 39.373373998453346
		 26 39.373373998453317 27 39.373373998453367 28 39.373373998453353 29 39.37337399845336
		 30 39.37337399845336 31 39.373373998453339 32 39.373373998453353 33 39.37337399845336
		 34 39.373373998453317 35 39.373373998453332 36 39.373373998453332 37 39.373373998453374
		 38 39.373373998453353 39 39.37337399845336 40 39.373373998453374;
createNode animCurveTL -n "L_lipcorner_jnt_translateY";
	rename -uid "F1316C84-41EC-3F70-958B-6D9768D3F4C1";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 3.7949294542846985 1 3.7949289783972233
		 2 3.7949294542846559 3 3.7949294542846985 4 3.7949294542846701 5 3.7949294542846985
		 6 3.7949294542846985 7 3.7949294542846843 8 3.7949294542846843 9 3.7949294542846701
		 10 3.7949294542846701 11 3.7949294542846985 12 3.7949294542846843 13 3.7949294542846985
		 14 3.7949294542846985 15 3.7949294542846701 16 3.7949294542846843 17 3.7949291056127947
		 18 3.7949294542847127 19 3.7949294542846701 20 3.7949294542846985 21 3.7949294542846843
		 22 3.7949294542846985 23 3.7949294542846985 24 3.7949294542846843 25 3.7949294542846985
		 26 3.7949294542846701 27 3.7949294542846985 28 3.7949294542846843 29 3.7949294542847127
		 30 3.7949294542846843 31 3.7949294542846985 32 3.7949294542846701 33 3.7949294542847127
		 34 3.7949294542846843 35 3.7949294542847127 36 3.7949294542846843 37 3.7949294542846843
		 38 3.7949294542846843 39 3.7949294542847127 40 3.7949294542847127;
createNode animCurveTL -n "L_lipcorner_jnt_translateZ";
	rename -uid "AA986BD4-4A8B-8BA8-BD88-848419CD9433";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -37.943752548978217 1 -37.943752549018633
		 2 -37.943752548978239 3 -37.943752548978232 4 -37.943752548978232 5 -37.943752548978239
		 6 -37.943752548978225 7 -37.943752548978232 8 -37.94375254897821 9 -37.943752548978217
		 10 -37.943752548978225 11 -37.943752548978239 12 -37.943752548978232 13 -37.943752548978232
		 14 -37.943752548978225 15 -37.943752548978232 16 -37.943752548978232 17 -37.943752548952894
		 18 -37.943752548978203 19 -37.943752548978217 20 -37.943752548978225 21 -37.943752548978239
		 22 -37.943752548978217 23 -37.943752548978239 24 -37.943752548978217 25 -37.943752548978239
		 26 -37.943752548978225 27 -37.943752548978232 28 -37.943752548978225 29 -37.943752548978239
		 30 -37.943752548978232 31 -37.943752548978239 32 -37.943752548978239 33 -37.943752548978239
		 34 -37.943752548978217 35 -37.943752548978239 36 -37.943752548978232 37 -37.943752548978232
		 38 -37.943752548978246 39 -37.943752548978239 40 -37.943752548978217;
createNode animCurveTA -n "L_lipcorner_jnt_rotateX";
	rename -uid "B79D4C13-4ED6-29D2-8406-7F8D4C11D9E5";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_lipcorner_jnt_rotateY";
	rename -uid "D06EF3FB-4FC5-8867-D9CD-B78FEEE2CB48";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_lipcorner_jnt_rotateZ";
	rename -uid "476CDCBB-4318-009E-25F5-93B6020DB3B5";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "L_lipcorner_jnt_scaleX";
	rename -uid "07BB4B3B-4460-92E3-347A-ACBB06E792AC";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_lipcorner_jnt_scaleY";
	rename -uid "1162F4E5-4292-9B99-512B-CD9940476BDA";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_lipcorner_jnt_scaleZ";
	rename -uid "1736BF0C-43BE-ADA3-520D-C2ADF9A8D314";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_lowerlip1_jnt_visibility";
	rename -uid "FE949D25-4077-96FB-069F-4BAD6A693D9A";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "L_lowerlip1_jnt_translateX";
	rename -uid "D10A1D7E-4C70-0576-E1FC-D5B414973627";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -22.725683616303385 1 -22.725683616320694
		 2 -22.725683616303385 3 -22.725683616303385 4 -22.725683616303414 5 -22.725683616303385
		 6 -22.725683616303385 7 -22.725683616303314 8 -22.725683616303371 9 -22.725683616303328
		 10 -22.725683616303371 11 -22.725683616303385 12 -22.725683616303385 13 -22.725683616303371
		 14 -22.725683616303328 15 -22.725683616303371 16 -22.725683616303343 17 -22.725683616288535
		 18 -22.725683616303328 19 -22.7256836163034 20 -22.725683616303414 21 -22.725683616303371
		 22 -22.725683616303343 23 -22.725683616303343 24 -22.725683616303371 25 -22.725683616303357
		 26 -22.725683616303328 27 -22.725683616303357 28 -22.725683616303357 29 -22.725683616303385
		 30 -22.7256836163034 31 -22.725683616303371 32 -22.725683616303343 33 -22.7256836163034
		 34 -22.725683616303357 35 -22.725683616303328 36 -22.725683616303343 37 -22.725683616303328
		 38 -22.725683616303357 39 -22.725683616303414 40 -22.725683616303357;
createNode animCurveTL -n "L_lowerlip1_jnt_translateY";
	rename -uid "99B33F84-4253-3607-4C01-B4947BA9F8AF";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -43.185460235760495 1 -43.185460235696013
		 2 -43.185460235760452 3 -43.185460235760502 4 -43.185460235760438 5 -43.185460235760466
		 6 -43.18546023576053 7 -43.185460235760459 8 -43.185460235760473 9 -43.185460235760473
		 10 -43.185460235760466 11 -43.185460235760431 12 -43.185460235760466 13 -43.185460235760473
		 14 -43.185460235760473 15 -43.185460235760488 16 -43.185460235760488 17 -43.18546023579961
		 18 -43.185460235760495 19 -43.185460235760445 20 -43.185460235760488 21 -43.185460235760459
		 22 -43.185460235760452 23 -43.185460235760452 24 -43.185460235760452 25 -43.185460235760473
		 26 -43.185460235760488 27 -43.185460235760459 28 -43.185460235760473 29 -43.185460235760459
		 30 -43.18546023576048 31 -43.185460235760488 32 -43.185460235760466 33 -43.185460235760459
		 34 -43.185460235760502 35 -43.185460235760473 36 -43.185460235760473 37 -43.185460235760488
		 38 -43.185460235760466 39 -43.185460235760445 40 -43.185460235760516;
createNode animCurveTL -n "L_lowerlip1_jnt_translateZ";
	rename -uid "F1F6F55B-43BC-23A4-3F9A-CFB3C6EE6984";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 22.048105564046686 1 22.048105564155097
		 2 22.048105564046711 3 22.048105564046711 4 22.048105564046701 5 22.048105564046715
		 6 22.048105564046711 7 22.048105564046693 8 22.048105564046693 9 22.04810556404669
		 10 22.048105564046729 11 22.048105564046693 12 22.048105564046715 13 22.048105564046708
		 14 22.048105564046697 15 22.048105564046715 16 22.048105564046711 17 22.048105563985377
		 18 22.048105564046701 19 22.048105564046697 20 22.048105564046693 21 22.048105564046693
		 22 22.048105564046708 23 22.048105564046725 24 22.048105564046704 25 22.048105564046693
		 26 22.048105564046693 27 22.048105564046715 28 22.048105564046701 29 22.048105564046711
		 30 22.048105564046715 31 22.048105564046701 32 22.048105564046704 33 22.048105564046715
		 34 22.048105564046697 35 22.048105564046711 36 22.048105564046704 37 22.048105564046704
		 38 22.048105564046708 39 22.048105564046711 40 22.048105564046686;
createNode animCurveTA -n "L_lowerlip1_jnt_rotateX";
	rename -uid "49142E06-43AA-491C-187E-DBA00C96DB63";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_lowerlip1_jnt_rotateY";
	rename -uid "0DF5480A-4AEC-0484-AEBD-E98E6EDFFA21";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_lowerlip1_jnt_rotateZ";
	rename -uid "9FF73025-4281-C336-D830-B398BD14E84C";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "L_lowerlip1_jnt_scaleX";
	rename -uid "89B3A4A5-4CA6-9C6F-2D25-DCA85D537047";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_lowerlip1_jnt_scaleY";
	rename -uid "1E6DD675-4DC2-4CD8-AC73-0E9865B72F5E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_lowerlip1_jnt_scaleZ";
	rename -uid "DABA5151-4454-EAAB-CC3E-58BFE062E48B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_lowerlip2_jnt_visibility";
	rename -uid "55774553-4E92-D8B0-CDF7-98A55EC4AA45";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "L_lowerlip2_jnt_translateX";
	rename -uid "65DFCB2A-4E07-AB12-65FD-F682490B044F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -18.22995055107539 1 -18.229950551105645
		 2 -18.229950551075362 3 -18.229950551075348 4 -18.22995055107539 5 -18.229950551075376
		 6 -18.229950551075362 7 -18.229950551075319 8 -18.229950551075333 9 -18.229950551075333
		 10 -18.229950551075348 11 -18.229950551075362 12 -18.229950551075348 13 -18.229950551075348
		 14 -18.229950551075333 15 -18.229950551075348 16 -18.229950551075333 17 -18.229950551049441
		 18 -18.229950551075333 19 -18.229950551075405 20 -18.229950551075362 21 -18.229950551075348
		 22 -18.229950551075348 23 -18.229950551075348 24 -18.229950551075362 25 -18.229950551075348
		 26 -18.229950551075319 27 -18.229950551075348 28 -18.229950551075362 29 -18.229950551075362
		 30 -18.229950551075348 31 -18.229950551075333 32 -18.229950551075348 33 -18.229950551075362
		 34 -18.229950551075362 35 -18.229950551075333 36 -18.229950551075319 37 -18.229950551075319
		 38 -18.229950551075348 39 -18.229950551075419 40 -18.229950551075376;
createNode animCurveTL -n "L_lowerlip2_jnt_translateY";
	rename -uid "D751C1A5-4787-70A5-51C8-EDB95CE99B66";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -27.625716391459811 1 -27.62571639134714
		 2 -27.62571639145979 3 -27.625716391459818 4 -27.625716391459761 5 -27.625716391459783
		 6 -27.625716391459846 7 -27.625716391459804 8 -27.625716391459804 9 -27.62571639145979
		 10 -27.625716391459783 11 -27.625716391459768 12 -27.62571639145979 13 -27.625716391459818
		 14 -27.625716391459783 15 -27.625716391459811 16 -27.625716391459818 17 -27.625716391528179
		 18 -27.625716391459797 19 -27.625716391459761 20 -27.625716391459811 21 -27.62571639145979
		 22 -27.62571639145979 23 -27.62571639145979 24 -27.62571639145979 25 -27.625716391459818
		 26 -27.625716391459818 27 -27.625716391459783 28 -27.625716391459811 29 -27.625716391459768
		 30 -27.62571639145979 31 -27.625716391459832 32 -27.625716391459818 33 -27.625716391459775
		 34 -27.625716391459811 35 -27.62571639145979 36 -27.625716391459797 37 -27.625716391459775
		 38 -27.625716391459797 39 -27.625716391459761 40 -27.625716391459832;
createNode animCurveTL -n "L_lowerlip2_jnt_translateZ";
	rename -uid "3A399ED3-408A-25E1-1AFB-17BECD36EE27";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 38.537800881108005 1 38.537800881174448
		 2 38.537800881108012 3 38.537800881108026 4 38.537800881107998 5 38.537800881108019
		 6 38.537800881108026 7 38.537800881107998 8 38.537800881108005 9 38.537800881107984
		 10 38.537800881108033 11 38.537800881108005 12 38.537800881108019 13 38.537800881108012
		 14 38.537800881108012 15 38.537800881108019 16 38.537800881108005 17 38.53780088107127
		 18 38.537800881107998 19 38.537800881107998 20 38.537800881108012 21 38.537800881108005
		 22 38.537800881108012 23 38.537800881108041 24 38.537800881107998 25 38.537800881108012
		 26 38.537800881107998 27 38.537800881108026 28 38.537800881108005 29 38.537800881108026
		 30 38.537800881108033 31 38.537800881107991 32 38.537800881108019 33 38.537800881108019
		 34 38.537800881108012 35 38.537800881108019 36 38.537800881108019 37 38.537800881108019
		 38 38.537800881108026 39 38.537800881108026 40 38.537800881108005;
createNode animCurveTA -n "L_lowerlip2_jnt_rotateX";
	rename -uid "56A89109-4CE8-16DB-69BB-34A33F06BF6E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_lowerlip2_jnt_rotateY";
	rename -uid "5317179E-4B92-7974-6EEA-BBACF50D1206";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_lowerlip2_jnt_rotateZ";
	rename -uid "BE52C6E3-42BD-6BDF-E136-4994F55AF8BA";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "L_lowerlip2_jnt_scaleX";
	rename -uid "3F03DB97-4A9B-2D5F-4FC7-728F38FE2501";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_lowerlip2_jnt_scaleY";
	rename -uid "F32C6144-43B9-91C5-693B-AF92C4F4AD55";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_lowerlip2_jnt_scaleZ";
	rename -uid "E43D25F8-4716-F2E0-DDE8-EBAA5F51508B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_scapula_jnt_visibility";
	rename -uid "8F976D69-4E53-7CB7-258C-F8B2F2AD1086";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "L_scapula_jnt_translateX";
	rename -uid "47D6E5B3-4AC6-5236-7DC6-3CB286BD0C87";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 2.2669785401953959 1 2.2669785401947991
		 2 2.266978540195403 3 2.2669785401947706 4 2.2669785401953888 5 2.2669785401947635
		 6 2.2669785401954101 7 2.266978540195403 8 2.2669785401953959 9 2.266978540195403
		 10 2.2669785401953959 11 2.266978540195403 12 2.2669785401954172 13 2.2669785401947777
		 14 2.2669785401947777 15 2.2669785401953888 16 2.2669785401953888 17 2.2669785401947493
		 18 2.2669785401954172 19 2.2669785401947848 20 2.2669785401954101 21 2.2669785401953888
		 22 2.2669785401953888 23 2.2669785401947564 24 2.2669785401954101 25 2.2669785401947706
		 26 2.2669785401947564 27 2.2669785401947564 28 2.2669785401953888 29 2.2669785401947777
		 30 2.2669785401954314 31 2.2669785401953888 32 2.2669785401947848 33 2.2669785401954172
		 34 2.266978540195403 35 2.2669785401947706 36 2.2669785401953888 37 2.2669785401947635
		 38 2.2669785401954101 39 2.2669785401947564 40 2.2669785401953959;
createNode animCurveTL -n "L_scapula_jnt_translateY";
	rename -uid "1204D143-403D-447F-AA71-F394C91686AC";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 39.838154710131519 1 39.838154710131491
		 2 39.838154710131519 3 39.838154710131519 4 39.838154710131505 5 39.838154710131576
		 6 39.838154710131491 7 39.838154710131505 8 39.838154710131562 9 39.838154710131505
		 10 39.838154710131505 11 39.838154710131548 12 39.838154710131491 13 39.838154710131505
		 14 39.838154710131576 15 39.838154710131519 16 39.838154710131491 17 39.838154710131562
		 18 39.838154710131477 19 39.838154710131462 20 39.838154710131448 21 39.838154710131462
		 22 39.838154710131505 23 39.838154710131505 24 39.838154710131548 25 39.838154710131519
		 26 39.838154710131505 27 39.838154710131505 28 39.838154710131477 29 39.838154710131505
		 30 39.838154710131477 31 39.838154710131491 32 39.838154710131505 33 39.838154710131477
		 34 39.838154710131519 35 39.838154710131519 36 39.838154710131548 37 39.838154710131576
		 38 39.838154710131519 39 39.838154710131576 40 39.838154710131519;
createNode animCurveTL -n "L_scapula_jnt_translateZ";
	rename -uid "79ADF4AD-41AB-2260-AC5F-AFAF288726D0";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 36.9564389674049 1 36.956438967404893
		 2 36.9564389674049 3 36.956438967404914 4 36.9564389674049 5 36.956438967404907 6 36.9564389674049
		 7 36.9564389674049 8 36.9564389674049 9 36.9564389674049 10 36.956438967404907 11 36.956438967404907
		 12 36.956438967404893 13 36.9564389674049 14 36.9564389674049 15 36.9564389674049
		 16 36.9564389674049 17 36.9564389674049 18 36.956438967404893 19 36.956438967404907
		 20 36.956438967404907 21 36.956438967404907 22 36.956438967404907 23 36.9564389674049
		 24 36.956438967404907 25 36.956438967404907 26 36.9564389674049 27 36.9564389674049
		 28 36.956438967404914 29 36.9564389674049 30 36.956438967404907 31 36.956438967404914
		 32 36.956438967404907 33 36.956438967404907 34 36.956438967404893 35 36.956438967404893
		 36 36.9564389674049 37 36.956438967404893 38 36.956438967404907 39 36.956438967404893
		 40 36.9564389674049;
createNode animCurveTA -n "L_scapula_jnt_rotateX";
	rename -uid "652664CE-4075-6D88-F8F0-CF860B49D13A";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -3.1805546814635176e-15 1 4.4209710072343099e-13
		 2 -6.3611093629270335e-15 3 4.4845821008635803e-13 4 -3.1805546814635168e-15 5 4.5163876476782165e-13
		 6 -6.3611093629270335e-15 7 -3.1805546814635168e-15 8 -6.361109362927032e-15 9 -6.361109362927032e-15
		 10 -6.361109362927032e-15 11 -3.1805546814635176e-15 12 -3.1805546814635176e-15 13 4.5163876476782165e-13
		 14 4.4845821008635803e-13 15 -6.3611093629270335e-15 16 -3.180554681463516e-15 17 4.4845821008635803e-13
		 18 -6.3611093629270335e-15 19 4.4845821008635803e-13 20 -6.3611093629270335e-15 21 -3.1805546814635168e-15
		 22 -6.3611093629270375e-15 23 4.5163876476782165e-13 24 -3.1805546814635168e-14 25 4.2937488199757701e-13
		 26 4.5163876476782165e-13 27 4.4845821008635803e-13 28 -6.3611093629270335e-15 29 4.4845821008635803e-13
		 30 -2.8624992133171654e-14 31 -3.1805546814635168e-15 32 4.4209710072343099e-13 33 -3.1805546814635168e-15
		 34 -6.361109362927032e-15 35 4.4845821008635803e-13 36 -3.1805546814635168e-15 37 4.3891654604196742e-13
		 38 -6.3611093629270335e-15 39 4.4845821008635803e-13 40 -3.1805546814635168e-15;
createNode animCurveTA -n "L_scapula_jnt_rotateY";
	rename -uid "2E3D32A3-48BC-5526-4040-43B1E78B8F68";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -9.5416640443905503e-15 1 -3.6258323368683829e-13
		 2 -6.3611093629270335e-15 3 -3.6576378836830176e-13 4 -9.5416640443905503e-15 5 -3.7212489773122875e-13
		 6 0 7 1.7655625192200639e-31 8 9.5416640443905519e-15 9 9.5416640443905519e-15 10 9.5416640443905519e-15
		 11 -9.5416640443905487e-15 12 -9.5416640443905503e-15 13 -3.7212489773122875e-13
		 14 -3.6576378836830176e-13 15 0 16 9.5416640443905503e-15 17 -3.6258323368683819e-13
		 18 -6.3611093629270335e-15 19 -3.6576378836830176e-13 20 -6.3611093629270335e-15
		 21 0 22 -2.5444437451708128e-14 23 -3.7212489773122875e-13 24 -3.1805546814635168e-15
		 25 -3.6894434304976538e-13 26 -3.7212489773122875e-13 27 -3.6258323368683819e-13
		 28 -6.3611093629270335e-15 29 -3.6258323368683819e-13 30 -6.3611093629270335e-15
		 31 0 32 -3.6894434304976533e-13 33 -9.5416640443905503e-15 34 9.5416640443905503e-15
		 35 -3.6576378836830176e-13 36 -9.5416640443905503e-15 37 -3.5304156964244773e-13
		 38 9.5416640443905503e-15 39 -3.6576378836830176e-13 40 -9.5416640443905503e-15;
createNode animCurveTA -n "L_scapula_jnt_rotateZ";
	rename -uid "995696A5-4316-DD21-E0EB-66BAA5F33371";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 6.3611093629270335e-15 1 -6.7427759247026693e-13
		 2 6.3611093629270335e-15 3 -6.8699981119612111e-13 4 6.3611093629270335e-15 5 -6.8699981119612111e-13
		 6 1.9083328088781101e-14 7 6.3611093629270335e-15 8 1.9083328088781101e-14 9 1.9083328088781101e-14
		 10 1.9083328088781101e-14 11 6.3611093629270335e-15 12 6.3611093629270335e-15 13 -6.8699981119612111e-13
		 14 -6.8699981119612111e-13 15 1.9083328088781101e-14 16 6.3611093629270335e-15 17 -6.9336092055904815e-13
		 18 6.3611093629270335e-15 19 -6.8699981119612111e-13 20 6.3611093629270335e-15 21 6.3611093629270335e-15
		 22 2.5444437451708128e-14 23 -6.8699981119612111e-13 24 1.2722218725854067e-14 25 -6.9336092055904805e-13
		 26 -6.8699981119612111e-13 27 -6.9336092055904815e-13 28 6.3611093629270335e-15 29 -6.9336092055904815e-13
		 30 1.2722218725854067e-14 31 6.3611093629270335e-15 32 -6.7427759247026703e-13 33 6.3611093629270335e-15
		 34 1.9083328088781101e-14 35 -6.8699981119612111e-13 36 6.3611093629270335e-15 37 -6.8063870183319397e-13
		 38 1.9083328088781101e-14 39 -6.8699981119612111e-13 40 6.3611093629270335e-15;
createNode animCurveTU -n "L_scapula_jnt_scaleX";
	rename -uid "4FE36FC9-4BC5-2773-A973-9AAD07671CC4";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_scapula_jnt_scaleY";
	rename -uid "0DE313DE-42FF-CB01-4D55-189BEA792685";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_scapula_jnt_scaleZ";
	rename -uid "9801AD58-496C-8D24-29DF-E7B520A6762A";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_upperlip1_jnt_visibility";
	rename -uid "C9049308-4342-3DDB-D044-0DA0826AA63D";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "L_upperlip1_jnt_translateX";
	rename -uid "E13E5DD1-4D47-EB0F-2BF8-E89A3AA5D9D8";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 41.141235128679021 1 41.141235459796775
		 2 41.141235128679 3 41.141235128678993 4 41.141235128678971 5 41.141235128678986
		 6 41.141235128679021 7 41.141235128679014 8 41.141235128678986 9 41.141235128678971
		 10 41.141235128678957 11 41.141235128679 12 41.141235128678993 13 41.141235128679014
		 14 41.141235128679 15 41.141235128679014 16 41.141235128678993 17 41.141235371213952
		 18 41.141235128678979 19 41.141235128678986 20 41.141235128678993 21 41.141235128678993
		 22 41.141235128678964 23 41.141235128678971 24 41.141235128678979 25 41.141235128679007
		 26 41.141235128678979 27 41.141235128679021 28 41.141235128679007 29 41.141235128679028
		 30 41.141235128679 31 41.141235128678979 32 41.141235128679014 33 41.141235128679028
		 34 41.141235128678971 35 41.141235128678971 36 41.141235128678986 37 41.141235128679007
		 38 41.141235128679014 39 41.141235128679007 40 41.141235128679014;
createNode animCurveTL -n "L_upperlip1_jnt_translateY";
	rename -uid "9F906374-4569-679E-35DD-A6AE70A75A04";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 27.391497693986651 1 27.391497196671395
		 2 27.391497693986636 3 27.391497693986707 4 27.391497693986665 5 27.391497693986693
		 6 27.391497693986665 7 27.391497693986636 8 27.391497693986636 9 27.391497693986665
		 10 27.391497693986651 11 27.391497693986651 12 27.391497693986636 13 27.391497693986679
		 14 27.391497693986665 15 27.391497693986651 16 27.391497693986679 17 27.391497329697359
		 18 27.391497693986665 19 27.391497693986651 20 27.391497693986665 21 27.391497693986665
		 22 27.391497693986679 23 27.391497693986651 24 27.391497693986651 25 27.391497693986651
		 26 27.391497693986651 27 27.391497693986651 28 27.391497693986665 29 27.391497693986679
		 30 27.391497693986665 31 27.391497693986679 32 27.391497693986651 33 27.391497693986665
		 34 27.391497693986665 35 27.391497693986679 36 27.391497693986665 37 27.391497693986665
		 38 27.391497693986651 39 27.391497693986665 40 27.391497693986679;
createNode animCurveTL -n "L_upperlip1_jnt_translateZ";
	rename -uid "D78B8147-430A-07AB-7A0C-6CBE36CD9EDF";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -17.555322453526852 1 -17.555322453504946
		 2 -17.555322453526873 3 -17.555322453526877 4 -17.555322453526866 5 -17.555322453526877
		 6 -17.555322453526873 7 -17.555322453526863 8 -17.555322453526863 9 -17.55532245352687
		 10 -17.555322453526863 11 -17.555322453526866 12 -17.555322453526877 13 -17.555322453526873
		 14 -17.55532245352687 15 -17.55532245352687 16 -17.555322453526866 17 -17.555322453540612
		 18 -17.555322453526859 19 -17.555322453526863 20 -17.555322453526863 21 -17.555322453526873
		 22 -17.55532245352687 23 -17.555322453526866 24 -17.555322453526866 25 -17.55532245352687
		 26 -17.555322453526866 27 -17.555322453526873 28 -17.55532245352687 29 -17.555322453526873
		 30 -17.55532245352687 31 -17.55532245352687 32 -17.55532245352687 33 -17.55532245352687
		 34 -17.555322453526866 35 -17.555322453526866 36 -17.555322453526866 37 -17.555322453526873
		 38 -17.555322453526877 39 -17.555322453526873 40 -17.555322453526848;
createNode animCurveTA -n "L_upperlip1_jnt_rotateX";
	rename -uid "A15BD7D2-4EB1-39C0-9966-39A07748EC27";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_upperlip1_jnt_rotateY";
	rename -uid "D45241C6-451B-B384-38B7-7CAB45C63519";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_upperlip1_jnt_rotateZ";
	rename -uid "57191914-4F94-D8D7-9477-F98D36BBE3F8";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "L_upperlip1_jnt_scaleX";
	rename -uid "ACAF0F70-4E92-0798-4E2E-2780628D0994";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_upperlip1_jnt_scaleY";
	rename -uid "AF67C155-470E-2209-F41D-39ABF618A69D";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_upperlip1_jnt_scaleZ";
	rename -uid "CA61FAEB-4665-C8B0-90C8-57B9B2C47797";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_upperlip2_jnt_visibility";
	rename -uid "ECCDC79A-45CD-1783-3FFF-BABC9910919F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "L_upperlip2_jnt_translateX";
	rename -uid "2AAA1A61-43B2-C635-8C4F-BC922701906D";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 38.007675996871221 1 38.00767617432777
		 2 38.007675996871207 3 38.007675996871214 4 38.007675996871185 5 38.0076759968712
		 6 38.007675996871228 7 38.007675996871257 8 38.007675996871228 9 38.007675996871228
		 10 38.007675996871228 11 38.007675996871228 12 38.007675996871235 13 38.007675996871221
		 14 38.0076759968712 15 38.007675996871228 16 38.007675996871228 17 38.007676126892747
		 18 38.0076759968712 19 38.007675996871221 20 38.007675996871207 21 38.007675996871214
		 22 38.007675996871185 23 38.007675996871214 24 38.007675996871193 25 38.007675996871214
		 26 38.007675996871185 27 38.007675996871228 28 38.007675996871214 29 38.007675996871235
		 30 38.007675996871214 31 38.007675996871207 32 38.007675996871214 33 38.007675996871242
		 34 38.007675996871214 35 38.007675996871207 36 38.007675996871214 37 38.007675996871228
		 38 38.007675996871221 39 38.007675996871228 40 38.007675996871235;
createNode animCurveTL -n "L_upperlip2_jnt_translateY";
	rename -uid "D3E6FCD9-41C5-4552-0C62-30B76797B5DE";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 14.682370520699465 1 14.68237006130542
		 2 14.682370520699479 3 14.682370520699521 4 14.68237052069945 5 14.682370520699507
		 6 14.682370520699479 7 14.682370520699479 8 14.68237052069945 9 14.68237052069945
		 10 14.682370520699465 11 14.682370520699465 12 14.68237052069945 13 14.682370520699493
		 14 14.68237052069945 15 14.682370520699436 16 14.682370520699465 17 14.682370184129809
		 18 14.682370520699507 19 14.682370520699465 20 14.682370520699479 21 14.682370520699479
		 22 14.682370520699493 23 14.682370520699465 24 14.682370520699465 25 14.682370520699465
		 26 14.682370520699465 27 14.682370520699465 28 14.682370520699479 29 14.682370520699465
		 30 14.682370520699479 31 14.682370520699465 32 14.682370520699436 33 14.682370520699479
		 34 14.68237052069945 35 14.682370520699493 36 14.68237052069945 37 14.682370520699479
		 38 14.682370520699436 39 14.682370520699479 40 14.682370520699493;
createNode animCurveTL -n "L_upperlip2_jnt_translateZ";
	rename -uid "9F3209FB-4700-7E6C-E68D-03AB20DCF7B2";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -31.825542046876109 1 -31.825542046884959
		 2 -31.825542046876127 3 -31.82554204687613 4 -31.825542046876116 5 -31.82554204687613
		 6 -31.825542046876127 7 -31.82554204687613 8 -31.825542046876109 9 -31.825542046876123
		 10 -31.825542046876123 11 -31.825542046876119 12 -31.82554204687613 13 -31.825542046876123
		 14 -31.825542046876116 15 -31.825542046876123 16 -31.825542046876119 17 -31.825542046870588
		 18 -31.825542046876112 19 -31.825542046876102 20 -31.825542046876123 21 -31.82554204687613
		 22 -31.825542046876109 23 -31.825542046876123 24 -31.825542046876119 25 -31.82554204687613
		 26 -31.825542046876116 27 -31.825542046876137 28 -31.825542046876116 29 -31.825542046876123
		 30 -31.825542046876134 31 -31.825542046876123 32 -31.82554204687613 33 -31.82554204687613
		 34 -31.825542046876116 35 -31.825542046876127 36 -31.825542046876123 37 -31.825542046876127
		 38 -31.825542046876134 39 -31.825542046876134 40 -31.825542046876109;
createNode animCurveTA -n "L_upperlip2_jnt_rotateX";
	rename -uid "CBBE7810-4AD2-BD42-7F5D-9AB2F22CA6D4";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_upperlip2_jnt_rotateY";
	rename -uid "2830B9A5-45B5-A5CD-2E9D-13A14E770D3C";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "L_upperlip2_jnt_rotateZ";
	rename -uid "2D3770F2-4AC6-DD21-C8A8-CA96D3BBA4B2";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "L_upperlip2_jnt_scaleX";
	rename -uid "FC2251D1-4E1E-F9CF-087F-61BF366E4216";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_upperlip2_jnt_scaleY";
	rename -uid "CD127EF5-47EA-8777-A68B-2195569A39C4";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "L_upperlip2_jnt_scaleZ";
	rename -uid "0B940C97-4BDA-09A8-7E1A-5FB2D3772068";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_B_ankle_jnt_visibility";
	rename -uid "924248C8-4119-6288-218D-899746BABC14";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "R_B_ankle_jnt_translateX";
	rename -uid "9C49BA18-4188-BF3E-D6EF-FE98B2A1582F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -19.586760394923367 1 -19.586760394923367
		 2 -19.586760394923367 3 -19.586760394923367 4 -19.586760394923367 5 -19.586760394923367
		 6 -19.586760394923367 7 -19.586760394923367 8 -19.586760394923367 9 -19.586760394923367
		 10 -19.586760394923367 11 -19.586760394923367 12 -19.586760394923367 13 -19.586760394923367
		 14 -19.586760394923367 15 -19.586760394923367 16 -19.586760394923367 17 -19.586760394923367
		 18 -19.586760394923367 19 -19.586760394923367 20 -19.586760394923367 21 -19.586760394923367
		 22 -19.586760394923367 23 -19.586760394923367 24 -19.586760394923367 25 -19.586760394923367
		 26 -19.586760394923367 27 -19.586760394923367 28 -19.586760394923367 29 -19.586760394923367
		 30 -19.586760394923367 31 -19.586760394923367 32 -19.586760394923367 33 -19.586760394923367
		 34 -19.586760394923367 35 -19.586760394923367 36 -19.586760394923367 37 -19.586760394923367
		 38 -19.586760394923367 39 -19.586760394923367 40 -19.586760394923367;
createNode animCurveTL -n "R_B_ankle_jnt_translateY";
	rename -uid "1EEB3325-42F0-7DFC-B2E1-39BEB9C65585";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -3.7406458865518744e-05 1 -3.7406458865518744e-05
		 2 -3.7406458865518744e-05 3 -3.7406458865518744e-05 4 -3.7406458865518744e-05 5 -3.7406458865518744e-05
		 6 -3.7406458865518744e-05 7 -3.7406458865518744e-05 8 -3.7406458865518744e-05 9 -3.7406458865518744e-05
		 10 -3.7406458865518744e-05 11 -3.7406458865518744e-05 12 -3.7406458865518744e-05
		 13 -3.7406458865518744e-05 14 -3.7406458865518744e-05 15 -3.7406458865518744e-05
		 16 -3.7406458865518744e-05 17 -3.7406458865518744e-05 18 -3.7406458865518744e-05
		 19 -3.7406458865518744e-05 20 -3.7406458865518744e-05 21 -3.7406458865518744e-05
		 22 -3.7406458865518744e-05 23 -3.7406458865518744e-05 24 -3.7406458865518744e-05
		 25 -3.7406458865518744e-05 26 -3.7406458865518744e-05 27 -3.7406458865518744e-05
		 28 -3.7406458865518744e-05 29 -3.7406458865518744e-05 30 -3.7406458865518744e-05
		 31 -3.7406458865518744e-05 32 -3.7406458865518744e-05 33 -3.7406458865518744e-05
		 34 -3.7406458865518744e-05 35 -3.7406458865518744e-05 36 -3.7406458865518744e-05
		 37 -3.7406458865518744e-05 38 -3.7406458865518744e-05 39 -3.7406458865518744e-05
		 40 -3.7406458865518744e-05;
createNode animCurveTL -n "R_B_ankle_jnt_translateZ";
	rename -uid "E8A039AE-4877-BE58-7AEC-E4820BBF5971";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -2.1508713729190276e-05 1 -2.1508713729190276e-05
		 2 -2.1508713729190276e-05 3 -2.1508713729190276e-05 4 -2.1508713729190276e-05 5 -2.1508713729190276e-05
		 6 -2.1508713729190276e-05 7 -2.1508713729190276e-05 8 -2.1508713729190276e-05 9 -2.1508713729190276e-05
		 10 -2.1508713729190276e-05 11 -2.1508713729190276e-05 12 -2.1508713729190276e-05
		 13 -2.1508713729190276e-05 14 -2.1508713729190276e-05 15 -2.1508713729190276e-05
		 16 -2.1508713729190276e-05 17 -2.1508713729190276e-05 18 -2.1508713729190276e-05
		 19 -2.1508713729190276e-05 20 -2.1508713729190276e-05 21 -2.1508713729190276e-05
		 22 -2.1508713729190276e-05 23 -2.1508713729190276e-05 24 -2.1508713729190276e-05
		 25 -2.1508713729190276e-05 26 -2.1508713729190276e-05 27 -2.1508713729190276e-05
		 28 -2.1508713729190276e-05 29 -2.1508713729190276e-05 30 -2.1508713729190276e-05
		 31 -2.1508713729190276e-05 32 -2.1508713729190276e-05 33 -2.1508713729190276e-05
		 34 -2.1508713729190276e-05 35 -2.1508713729190276e-05 36 -2.1508713729190276e-05
		 37 -2.1508713729190276e-05 38 -2.1508713729190276e-05 39 -2.1508713729190276e-05
		 40 -2.1508713729190276e-05;
createNode animCurveTA -n "R_B_ankle_jnt_rotateX";
	rename -uid "5CA5A7C9-48A7-F1A0-1BC8-86A43A30EE71";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -0.51751404406078771 1 -0.46839970635687445
		 2 -0.34163969043723424 3 -0.17346246642709034 4 0.0068400279174698512 5 0.15131283052116698
		 6 0.21697550905703039 7 0.17894822270131161 8 0.078649431042491572 9 -0.10217093902406219
		 10 -0.32317479276921734 11 -0.45989295026281146 12 -0.53195450716039838 13 -0.57553444152079647
		 14 -0.58999162458870891 15 -0.50295366797418273 16 -0.13979617090177707 17 0.41294058544207507
		 18 0.84373464454709579 19 1.1460602081347826 20 1.4818667293057761 21 1.8018772418361251
		 22 2.0991149915009122 23 2.3765952821796472 24 2.6074598514142773 25 2.9099724380800316
		 26 3.2710088996484248 27 3.5551044776656409 28 3.7577851018020487 29 3.8821441152966369
		 30 3.9480483928087122 31 3.9611888817587104 32 3.3257322396054558 33 2.1530485957066579
		 34 1.4138880965588037 35 1.2633839718168376 36 1.1322999042448898 37 0.79920019110858553
		 38 0.23401500377224838 39 -0.29085108367603918 40 -0.51751404406078771;
createNode animCurveTA -n "R_B_ankle_jnt_rotateY";
	rename -uid "2323D979-4B53-AED4-35DF-00BB5E6F73FC";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -0.61786033640608318 1 -0.51624786064640216
		 2 -0.37607239312596752 3 -0.30150624201027487 4 -0.33135224459892382 5 -0.3210402642553295
		 6 -0.26574159172088641 7 -0.36864744893711943 8 -0.47338157053357693 9 -0.50841055307659566
		 10 -0.47667293152711793 11 -0.45322478240548675 12 -0.474320639021032 13 -0.54070518500899079
		 14 -0.61108152573327257 15 -0.51030742031359178 16 0.022812652024102643 17 0.81596790851246881
		 18 1.4332972046811043 19 1.8569607787761995 20 2.3006040490881814 21 2.7053305544867401
		 22 3.0910729660062568 23 3.4690606932386552 24 3.8082158090648464 25 4.2921193077253639
		 26 4.8788775719588937 27 5.3574414369750256 28 5.7086961989708653 29 5.9156913933197615
		 30 5.9298686890862342 31 5.7388500884709339 32 4.5137621457912642 33 2.4077377465336793
		 34 1.0447778362840741 35 0.70644812515710376 36 0.63150203365816515 37 0.45261472994803087
		 38 0.022419617921656618 39 -0.41485672920678268 40 -0.61786033640608318;
createNode animCurveTA -n "R_B_ankle_jnt_rotateZ";
	rename -uid "5560C24E-46A5-D848-F878-48B1B4B36220";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 3.8391871052652058 1 3.6342798187557506
		 2 3.9897028182976073 3 5.2690948418121621 4 7.0559091074854576 5 8.2547355997142287
		 6 8.5160123929052531 7 8.77401522980721 8 8.4732690853158452 9 7.0908987758632609
		 10 5.0287762506299396 11 3.445318399634802 12 2.7973162481242264 13 2.7804692432492133
		 14 3.0896924091847375 15 3.2372307045877107 16 3.1825926080322251 17 3.2101476931230599
		 18 3.1411827307028983 19 2.8756255788115759 20 2.4879678733346102 21 2.0235531773750717
		 22 1.4926755691751727 23 1.2175011566897633 24 1.2200488751382748 25 1.1961011703506554
		 26 1.3469676035742664 27 1.6760422804153814 28 2.1130933125245299 29 2.6973148023892595
		 30 3.941320188400371 31 5.9161468443783543 32 8.1770776624762878 33 10.143560610448825
		 34 11.552942264649056 35 12.201627208470565 36 11.459412875097696 37 9.5195453373699195
		 38 7.0221901686700052 39 4.8019702210515947 40 3.8391871052652058;
createNode animCurveTU -n "R_B_ankle_jnt_scaleX";
	rename -uid "006DA165-4C62-3E05-8FB2-EE988AC4B56F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_B_ankle_jnt_scaleY";
	rename -uid "9A2D353D-483D-D85F-AF42-E28C223B1CA0";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_B_ankle_jnt_scaleZ";
	rename -uid "9DD04391-42F4-1C12-ECAB-8BAC653ED931";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_B_hip_jnt_visibility";
	rename -uid "78BDB13E-40CE-C0E6-9471-08930E37FAEA";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "R_B_hip_jnt_translateX";
	rename -uid "D7E3AEED-4E05-E707-8CA2-7EACD912CCED";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 2.942121990975231 1 2.942121990975231
		 2 2.942121990975231 3 2.942121990975231 4 2.942121990975231 5 2.942121990975231 6 2.942121990975231
		 7 2.942121990975231 8 2.942121990975231 9 2.942121990975231 10 2.942121990975231
		 11 2.942121990975231 12 2.942121990975231 13 2.942121990975231 14 2.942121990975231
		 15 2.942121990975231 16 2.942121990975231 17 2.942121990975231 18 2.942121990975231
		 19 2.942121990975231 20 2.942121990975231 21 2.942121990975231 22 2.942121990975231
		 23 2.942121990975231 24 2.942121990975231 25 2.942121990975231 26 2.942121990975231
		 27 2.942121990975231 28 2.942121990975231 29 2.942121990975231 30 2.942121990975231
		 31 2.942121990975231 32 2.942121990975231 33 2.942121990975231 34 2.942121990975231
		 35 2.942121990975231 36 2.942121990975231 37 2.942121990975231 38 2.942121990975231
		 39 2.942121990975231 40 2.942121990975231;
createNode animCurveTL -n "R_B_hip_jnt_translateY";
	rename -uid "56812B08-4500-F843-B8F8-D187EED4EC25";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -16.990776516016837 1 -16.990776516016837
		 2 -16.990776516016837 3 -16.990776516016837 4 -16.990776516016837 5 -16.990776516016837
		 6 -16.990776516016837 7 -16.990776516016837 8 -16.990776516016837 9 -16.990776516016837
		 10 -16.990776516016837 11 -16.990776516016837 12 -16.990776516016837 13 -16.990776516016837
		 14 -16.990776516016837 15 -16.990776516016837 16 -16.990776516016837 17 -16.990776516016837
		 18 -16.990776516016837 19 -16.990776516016837 20 -16.990776516016837 21 -16.990776516016837
		 22 -16.990776516016837 23 -16.990776516016837 24 -16.990776516016837 25 -16.990776516016837
		 26 -16.990776516016837 27 -16.990776516016837 28 -16.990776516016837 29 -16.990776516016837
		 30 -16.990776516016837 31 -16.990776516016837 32 -16.990776516016837 33 -16.990776516016837
		 34 -16.990776516016837 35 -16.990776516016837 36 -16.990776516016837 37 -16.990776516016837
		 38 -16.990776516016837 39 -16.990776516016837 40 -16.990776516016837;
createNode animCurveTL -n "R_B_hip_jnt_translateZ";
	rename -uid "A8AAB4EA-4E48-A13D-6841-2AB081C08205";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -33.69479999999993 1 -33.69479999999993
		 2 -33.69479999999993 3 -33.69479999999993 4 -33.69479999999993 5 -33.69479999999993
		 6 -33.69479999999993 7 -33.69479999999993 8 -33.69479999999993 9 -33.69479999999993
		 10 -33.69479999999993 11 -33.69479999999993 12 -33.69479999999993 13 -33.69479999999993
		 14 -33.69479999999993 15 -33.69479999999993 16 -33.69479999999993 17 -33.69479999999993
		 18 -33.69479999999993 19 -33.69479999999993 20 -33.69479999999993 21 -33.69479999999993
		 22 -33.69479999999993 23 -33.69479999999993 24 -33.69479999999993 25 -33.69479999999993
		 26 -33.69479999999993 27 -33.69479999999993 28 -33.69479999999993 29 -33.69479999999993
		 30 -33.69479999999993 31 -33.69479999999993 32 -33.69479999999993 33 -33.69479999999993
		 34 -33.69479999999993 35 -33.69479999999993 36 -33.69479999999993 37 -33.69479999999993
		 38 -33.69479999999993 39 -33.69479999999993 40 -33.69479999999993;
createNode animCurveTA -n "R_B_hip_jnt_rotateX";
	rename -uid "77C5A548-496C-ECC5-A444-39914F8492D9";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.14635319118935339 1 0.13527133325713658
		 2 0.13426778002179776 3 0.17197401074469928 4 0.24708029284576724 5 0.35857156373566629
		 6 0.56442818696010699 7 0.86040196014148396 8 1.1589510636886859 9 1.3800190070876655
		 10 1.4544925124067891 11 1.3881727905301091 12 1.2178787004349236 13 0.94869054972573408
		 14 0.58716674441043992 15 0.13886494988175044 16 -0.63085318339191576 17 -1.6287093932714638
		 18 -2.3930224273497696 19 -2.87410084645659 20 -3.2787642346649477 21 -3.5969120977143891
		 22 -3.8303610480247396 23 -3.9991164381477584 24 -4.1185357631336563 25 -4.0960490675669883
		 26 -3.8090641763157085 27 -3.2946362212582772 28 -2.6848113906864239 29 -2.1016422827834198
		 30 -1.6674449973728405 31 -1.501607845799211 32 -1.6180916663576639 33 -1.7738112687290901
		 34 -1.840748756962211 35 -1.8310884434587626 36 -1.7924159346936888 37 -1.4609670646454662
		 38 -0.80384231308598564 39 -0.15114180965610668 40 0.14635319118935342;
createNode animCurveTA -n "R_B_hip_jnt_rotateY";
	rename -uid "97ECE1EA-4B30-7980-1DC5-FC9A888FA352";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.40092218461262902 1 0.25542874265116455
		 2 -0.03198516884093644 3 -0.30266055514408169 4 -0.40221198972197181 5 -0.45799172807790972
		 6 -0.49431214649047261 7 -0.33115690283703719 8 -0.26508596043381827 9 -0.14309066351418689
		 10 0.13805302692718527 11 0.31345204878079025 12 0.37263562640691333 13 0.38826503929793932
		 14 0.38275767562192076 15 0.24804132980231491 16 -0.27400429787969149 17 -1.0610823672885032
		 18 -1.6576007588817154 19 -1.9704212911336936 20 -2.230065938516709 21 -2.4486260400492386
		 22 -2.7165834830491056 23 -3.1241097986434889 24 -3.608840064341952 25 -4.2722852432330445
		 26 -5.0496571828289554 27 -5.717033261766252 28 -6.2692064631694384 29 -6.6892052720296213
		 30 -6.9327832950260921 31 -6.9534011265823414 32 -5.749593648635841 33 -3.4667155626796093
		 34 -1.9671241527337673 35 -1.5834000174076972 36 -1.4003455918433478 37 -1.0607980801151216
		 38 -0.45045694872681169 39 0.13414178168922755 40 0.40092218461262902;
createNode animCurveTA -n "R_B_hip_jnt_rotateZ";
	rename -uid "D8FFB009-4D2D-AAE7-2716-DEBA164AC901";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.3613019746865498 1 -1.2463222518567525
		 2 -1.1015993552764789 3 -1.1231058055193874 4 -1.1068017061545969 5 -1.1519664807672554
		 6 -1.5108689653038712 7 -1.986804789757848 8 -1.7374633556438075 9 -1.3136492082567994
		 10 -1.4072248632840638 11 -1.4110751350766153 12 -1.2684483168365988 13 -1.1011119773176037
		 14 -1.0617591432632021 15 -1.3085684067252492 16 -2.1155757101422168 17 -3.363212179126212
		 18 -4.5789849540830359 19 -6.2382891197046568 20 -8.7963710119492102 21 -11.678701128019759
		 22 -14.229674271751023 23 -15.701889292163518 24 -15.958722682723495 25 -15.523024734982517
		 26 -14.376497192066305 27 -12.627315705910693 28 -10.635659154423541 29 -8.7155715590004004
		 30 -7.1670432420997887 31 -6.2511676625261572 32 -5.6312582196329695 33 -5.0937122028777226
		 34 -4.9185903475370489 35 -4.9645630799173679 36 -4.7427996534295804 37 -3.9772641894954077
		 38 -2.821769347192999 39 -1.8000863563627048 40 -1.3613019746865498;
createNode animCurveTU -n "R_B_hip_jnt_scaleX";
	rename -uid "3510D527-45C5-B1E7-4FAB-7CBADB142E82";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_B_hip_jnt_scaleY";
	rename -uid "90AC763F-45F7-22C4-843B-AC897A2C2CAD";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_B_hip_jnt_scaleZ";
	rename -uid "D97E3783-4217-31F3-1D7F-2F8D79824423";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_B_knee_jnt_visibility";
	rename -uid "053841FE-45BF-40C0-44A4-6DBA8B2B1660";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "R_B_knee_jnt_translateX";
	rename -uid "F79E42AC-493E-EADA-D97E-D1BAF0FF4858";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -46.336809573908354 1 -46.336809573908354
		 2 -46.336809573908354 3 -46.336809573908354 4 -46.336809573908354 5 -46.336809573908354
		 6 -46.336809573908354 7 -46.336809573908354 8 -46.336809573908354 9 -46.336809573908354
		 10 -46.336809573908354 11 -46.336809573908354 12 -46.336809573908354 13 -46.336809573908354
		 14 -46.336809573908354 15 -46.336809573908354 16 -46.336809573908354 17 -46.336809573908354
		 18 -46.336809573908354 19 -46.336809573908354 20 -46.336809573908354 21 -46.336809573908354
		 22 -46.336809573908354 23 -46.336809573908354 24 -46.336809573908354 25 -46.336809573908354
		 26 -46.336809573908354 27 -46.336809573908354 28 -46.336809573908354 29 -46.336809573908354
		 30 -46.336809573908354 31 -46.336809573908354 32 -46.336809573908354 33 -46.336809573908354
		 34 -46.336809573908354 35 -46.336809573908354 36 -46.336809573908354 37 -46.336809573908354
		 38 -46.336809573908354 39 -46.336809573908354 40 -46.336809573908354;
createNode animCurveTL -n "R_B_knee_jnt_translateY";
	rename -uid "3D14266E-43A6-257B-3D31-8ABABDBC6AAB";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -3.3403376946239405e-05 1 -3.3403376946239405e-05
		 2 -3.3403376946239405e-05 3 -3.3403376946239405e-05 4 -3.3403376946239405e-05 5 -3.3403376946239405e-05
		 6 -3.3403376946239405e-05 7 -3.3403376946239405e-05 8 -3.3403376946239405e-05 9 -3.3403376946239405e-05
		 10 -3.3403376946239405e-05 11 -3.3403376946239405e-05 12 -3.3403376946239405e-05
		 13 -3.3403376946239405e-05 14 -3.3403376946239405e-05 15 -3.3403376946239405e-05
		 16 -3.3403376946239405e-05 17 -3.3403376946239405e-05 18 -3.3403376946239405e-05
		 19 -3.3403376946239405e-05 20 -3.3403376946239405e-05 21 -3.3403376946239405e-05
		 22 -3.3403376946239405e-05 23 -3.3403376946239405e-05 24 -3.3403376946239405e-05
		 25 -3.3403376946239405e-05 26 -3.3403376946239405e-05 27 -3.3403376946239405e-05
		 28 -3.3403376946239405e-05 29 -3.3403376946239405e-05 30 -3.3403376946239405e-05
		 31 -3.3403376946239405e-05 32 -3.3403376946239405e-05 33 -3.3403376946239405e-05
		 34 -3.3403376946239405e-05 35 -3.3403376946239405e-05 36 -3.3403376946239405e-05
		 37 -3.3403376946239405e-05 38 -3.3403376946239405e-05 39 -3.3403376946239405e-05
		 40 -3.3403376946239405e-05;
createNode animCurveTL -n "R_B_knee_jnt_translateZ";
	rename -uid "9CAFBC3C-4952-DA94-F4CD-F298021E3E97";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.5450559992302715e-05 1 -1.5450559992302715e-05
		 2 -1.5450559992302715e-05 3 -1.5450559992302715e-05 4 -1.5450559992302715e-05 5 -1.5450559992302715e-05
		 6 -1.5450559992302715e-05 7 -1.5450559992302715e-05 8 -1.5450559992302715e-05 9 -1.5450559992302715e-05
		 10 -1.5450559992302715e-05 11 -1.5450559992302715e-05 12 -1.5450559992302715e-05
		 13 -1.5450559992302715e-05 14 -1.5450559992302715e-05 15 -1.5450559992302715e-05
		 16 -1.5450559992302715e-05 17 -1.5450559992302715e-05 18 -1.5450559992302715e-05
		 19 -1.5450559992302715e-05 20 -1.5450559992302715e-05 21 -1.5450559992302715e-05
		 22 -1.5450559992302715e-05 23 -1.5450559992302715e-05 24 -1.5450559992302715e-05
		 25 -1.5450559992302715e-05 26 -1.5450559992302715e-05 27 -1.5450559992302715e-05
		 28 -1.5450559992302715e-05 29 -1.5450559992302715e-05 30 -1.5450559992302715e-05
		 31 -1.5450559992302715e-05 32 -1.5450559992302715e-05 33 -1.5450559992302715e-05
		 34 -1.5450559992302715e-05 35 -1.5450559992302715e-05 36 -1.5450559992302715e-05
		 37 -1.5450559992302715e-05 38 -1.5450559992302715e-05 39 -1.5450559992302715e-05
		 40 -1.5450559992302715e-05;
createNode animCurveTA -n "R_B_knee_jnt_rotateX";
	rename -uid "6BD1761A-499B-75D8-71F1-4A86303BA8FC";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -2.0310505850085857e-06 1 -2.3861891542465937e-06
		 2 -2.0158999134105048e-06 3 -3.8373734095763174e-07 4 2.0658454253918981e-06 5 3.6356107939154224e-06
		 6 3.8029815777272119e-06 7 4.1282450056957302e-06 8 4.6825052186736992e-06 9 3.6263474677974136e-06
		 10 5.2309300924285289e-07 11 -1.863085036149811e-06 12 -2.8507049445567838e-06 13 -2.9462542154804588e-06
		 14 -2.6685348482463432e-06 15 -2.8381002168128877e-06 16 -3.8516117980496043e-06
		 17 -5.2590787419761759e-06 18 -6.8105873782638112e-06 19 -9.2381936331802317e-06
		 20 -1.2909743860074758e-05 21 -1.7002284325416815e-05 22 -2.0718846785460125e-05
		 23 -2.2740680578336901e-05 24 -2.2943550405653242e-05 25 -2.2438699204812431e-05
		 26 -2.0881203901674759e-05 27 -1.83370098475369e-05 28 -1.5299968935649854e-05 29 -1.207141632732038e-05
		 30 -8.2242452900223716e-06 31 -3.988649478602267e-06 32 2.7841675178639274e-07 33 4.0482205742508674e-06
		 34 6.4926372184965955e-06 35 7.4445256958857154e-06 36 6.4411186992007716e-06 37 4.1459943838641001e-06
		 38 1.4144130884701666e-06 39 -9.9507819737127658e-07 40 -2.0310505850085857e-06;
createNode animCurveTA -n "R_B_knee_jnt_rotateY";
	rename -uid "EE1C542F-473C-DC0E-7118-16A6AA6B431C";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.17166104472142435 1 0.20167677070395112
		 2 0.17038053495275859 3 0.032432846987476706 4 -0.20728900289166707 5 -0.36480083510117678
		 6 -0.38159498749181903 7 -0.4142322462295725 8 -0.46984727215482086 9 -0.36387134366350155
		 10 -0.052487677428713103 11 0.15746487363286293 12 0.24093687896654675 13 0.24901254571270967
		 14 0.22554016296130733 15 0.23987154817226861 16 0.32553187497876318 17 0.44448865911237179
		 18 0.57561960945171164 19 0.7807968851764594 20 1.0911102532175776 21 1.4370050216840911
		 22 1.7511227494120611 23 1.922004806067898 24 1.9391510291883975 25 1.8964818385710536
		 26 1.7648449050260457 27 1.5498138208516972 28 1.2931281333339568 29 1.0202561931791161
		 30 0.69509964397293744 31 0.33711407366130935 32 -0.027936616242810026 33 -0.4062025144803873
		 34 -0.6514777333372721 35 -0.7469911783011548 36 -0.64630831341648565 37 -0.41601323664504386
		 38 -0.14192362854557941 39 0.084102367612642698 40 0.17166104472142435;
createNode animCurveTA -n "R_B_knee_jnt_rotateZ";
	rename -uid "0B2932F7-402E-3DBC-FA25-ECBF3AC5A50F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.5502391362761618 1 1.8213056044860387
		 2 1.5386750894594186 3 0.29289504081975154 4 -1.520745136732184 5 -2.6763074167802623
		 6 -2.7995152339694638 7 -3.038953659594231 8 -3.4469650785562727 9 -2.6694883402071676
		 10 -0.38506808887396038 11 1.4220361415753295 12 2.1758563787859626 13 2.2487862311105955
		 14 2.0368114850525805 15 2.1662355734771617 16 2.9398181370525798 17 4.0140948466505115
		 18 5.1983141989387676 19 7.0512322166474828 20 9.8536148330860875 21 12.97732649393588
		 22 15.814065578870876 23 17.357269818045452 24 17.512114186864583 25 17.12677662052517
		 26 15.937987827517286 27 13.99608188872997 28 11.678000933567921 29 9.2137449254204551
		 30 6.2773162859819553 31 3.0444148305595267 32 -0.20495285661733748 33 -2.980044719242875
		 34 -4.779470115835025 35 -5.4801903899215043 36 -4.7415454305184053 37 -3.052019632584174
		 38 -1.0412017274783802 39 0.75951292233007928 40 1.5502391362761618;
createNode animCurveTU -n "R_B_knee_jnt_scaleX";
	rename -uid "74BFFA95-46E4-661A-DCB7-E494FE0CE369";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_B_knee_jnt_scaleY";
	rename -uid "DD69D7FC-4FF8-9F28-69CB-BC90594C6039";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_B_knee_jnt_scaleZ";
	rename -uid "601ED622-4F65-0059-CBFC-7185E5B67A0C";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_B_toe_jnt_visibility";
	rename -uid "7B938FEB-43D0-1CAA-243A-45B57E342C72";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "R_B_toe_jnt_translateX";
	rename -uid "21384780-4D06-2360-535F-7491186EA672";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -19.266184439749601 1 -19.266184439749601
		 2 -19.266184439749601 3 -19.266184439749601 4 -19.266184439749601 5 -19.266184439749601
		 6 -19.266184439749601 7 -19.266184439749601 8 -19.266184439749601 9 -19.266184439749601
		 10 -19.266184439749601 11 -19.266184439749601 12 -19.266184439749601 13 -19.266184439749601
		 14 -19.266184439749601 15 -19.266184439749601 16 -19.266184439749601 17 -19.266184439749601
		 18 -19.266184439749601 19 -19.266184439749601 20 -19.266184439749601 21 -19.266184439749601
		 22 -19.266184439749601 23 -19.266184439749601 24 -19.266184439749601 25 -19.266184439749601
		 26 -19.266184439749601 27 -19.266184439749601 28 -19.266184439749601 29 -19.266184439749601
		 30 -19.266184439749601 31 -19.266184439749601 32 -19.266184439749601 33 -19.266184439749601
		 34 -19.266184439749601 35 -19.266184439749601 36 -19.266184439749601 37 -19.266184439749601
		 38 -19.266184439749601 39 -19.266184439749601 40 -19.266184439749601;
createNode animCurveTL -n "R_B_toe_jnt_translateY";
	rename -uid "DE34A549-4BDD-E198-62CE-FDB15043E916";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.4150770027754334e-05 1 -1.4150770027754334e-05
		 2 -1.4150770027754334e-05 3 -1.4150770027754334e-05 4 -1.4150770027754334e-05 5 -1.4150770027754334e-05
		 6 -1.4150770027754334e-05 7 -1.4150770027754334e-05 8 -1.4150770027754334e-05 9 -1.4150770027754334e-05
		 10 -1.4150770027754334e-05 11 -1.4150770027754334e-05 12 -1.4150770027754334e-05
		 13 -1.4150770027754334e-05 14 -1.4150770027754334e-05 15 -1.4150770027754334e-05
		 16 -1.4150770027754334e-05 17 -1.4150770027754334e-05 18 -1.4150770027754334e-05
		 19 -1.4150770027754334e-05 20 -1.4150770027754334e-05 21 -1.4150770027754334e-05
		 22 -1.4150770027754334e-05 23 -1.4150770027754334e-05 24 -1.4150770027754334e-05
		 25 -1.4150770027754334e-05 26 -1.4150770027754334e-05 27 -1.4150770027754334e-05
		 28 -1.4150770027754334e-05 29 -1.4150770027754334e-05 30 -1.4150770027754334e-05
		 31 -1.4150770027754334e-05 32 -1.4150770027754334e-05 33 -1.4150770027754334e-05
		 34 -1.4150770027754334e-05 35 -1.4150770027754334e-05 36 -1.4150770027754334e-05
		 37 -1.4150770027754334e-05 38 -1.4150770027754334e-05 39 -1.4150770027754334e-05
		 40 -1.4150770027754334e-05;
createNode animCurveTL -n "R_B_toe_jnt_translateZ";
	rename -uid "32A1BC42-48D0-4600-180C-E788F0BD1708";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -3.0973806559586592e-05 1 -3.0973806559586592e-05
		 2 -3.0973806559586592e-05 3 -3.0973806559586592e-05 4 -3.0973806559586592e-05 5 -3.0973806559586592e-05
		 6 -3.0973806559586592e-05 7 -3.0973806559586592e-05 8 -3.0973806559586592e-05 9 -3.0973806559586592e-05
		 10 -3.0973806559586592e-05 11 -3.0973806559586592e-05 12 -3.0973806559586592e-05
		 13 -3.0973806559586592e-05 14 -3.0973806559586592e-05 15 -3.0973806559586592e-05
		 16 -3.0973806559586592e-05 17 -3.0973806559586592e-05 18 -3.0973806559586592e-05
		 19 -3.0973806559586592e-05 20 -3.0973806559586592e-05 21 -3.0973806559586592e-05
		 22 -3.0973806559586592e-05 23 -3.0973806559586592e-05 24 -3.0973806559586592e-05
		 25 -3.0973806559586592e-05 26 -3.0973806559586592e-05 27 -3.0973806559586592e-05
		 28 -3.0973806559586592e-05 29 -3.0973806559586592e-05 30 -3.0973806559586592e-05
		 31 -3.0973806559586592e-05 32 -3.0973806559586592e-05 33 -3.0973806559586592e-05
		 34 -3.0973806559586592e-05 35 -3.0973806559586592e-05 36 -3.0973806559586592e-05
		 37 -3.0973806559586592e-05 38 -3.0973806559586592e-05 39 -3.0973806559586592e-05
		 40 -3.0973806559586592e-05;
createNode animCurveTA -n "R_B_toe_jnt_rotateX";
	rename -uid "70A4549D-44FB-5D12-40F1-D2B36B326F44";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_B_toe_jnt_rotateY";
	rename -uid "C38A5E8F-4539-EB12-80F4-D292A5B272FC";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_B_toe_jnt_rotateZ";
	rename -uid "2B089225-4BC2-EAFE-BD6C-D59F8D01EA0C";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "R_B_toe_jnt_scaleX";
	rename -uid "A118ADE0-410A-4B67-08F0-1F8B3F13B852";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_B_toe_jnt_scaleY";
	rename -uid "875013D2-4A73-350C-54E4-20BBE1636397";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_B_toe_jnt_scaleZ";
	rename -uid "206A50A0-4848-DFAA-BAD8-6C8ACB088551";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_F_ankle_jnt_visibility";
	rename -uid "C9693F58-46D8-1C0D-E599-489ED0BB913B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "R_F_ankle_jnt_translateX";
	rename -uid "35DF49AD-4A73-9FFF-F7D8-C0A77C6734FA";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -28.109151207520913 1 -28.109151207520913
		 2 -28.109151207520913 3 -28.109151207520913 4 -28.109151207520913 5 -28.109151207520913
		 6 -28.109151207520913 7 -28.109151207520913 8 -28.109151207520913 9 -28.109151207520913
		 10 -28.109151207520913 11 -28.109151207520913 12 -28.109151207520913 13 -28.109151207520913
		 14 -28.109151207520913 15 -28.109151207520913 16 -28.109151207520913 17 -28.109151207520913
		 18 -28.109151207520913 19 -28.109151207520913 20 -28.109151207520913 21 -28.109151207520913
		 22 -28.109151207520913 23 -28.109151207520913 24 -28.109151207520913 25 -28.109151207520913
		 26 -28.109151207520913 27 -28.109151207520913 28 -28.109151207520913 29 -28.109151207520913
		 30 -28.109151207520913 31 -28.109151207520913 32 -28.109151207520913 33 -28.109151207520913
		 34 -28.109151207520913 35 -28.109151207520913 36 -28.109151207520913 37 -28.109151207520913
		 38 -28.109151207520913 39 -28.109151207520913 40 -28.109151207520913;
createNode animCurveTL -n "R_F_ankle_jnt_translateY";
	rename -uid "24378F92-4B2D-76F9-08BA-4DB5B673DAE5";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -3.6305016212878627e-05 1 -3.6305016212878627e-05
		 2 -3.6305016212878627e-05 3 -3.6305016212878627e-05 4 -3.6305016212878627e-05 5 -3.6305016212878627e-05
		 6 -3.6305016212878627e-05 7 -3.6305016212878627e-05 8 -3.6305016212878627e-05 9 -3.6305016212878627e-05
		 10 -3.6305016212878627e-05 11 -3.6305016212878627e-05 12 -3.6305016212878627e-05
		 13 -3.6305016212878627e-05 14 -3.6305016212878627e-05 15 -3.6305016212878627e-05
		 16 -3.6305016212878627e-05 17 -3.6305016212878627e-05 18 -3.6305016212878627e-05
		 19 -3.6305016212878627e-05 20 -3.6305016212878627e-05 21 -3.6305016212878627e-05
		 22 -3.6305016212878627e-05 23 -3.6305016212878627e-05 24 -3.6305016212878627e-05
		 25 -3.6305016212878627e-05 26 -3.6305016212878627e-05 27 -3.6305016212878627e-05
		 28 -3.6305016212878627e-05 29 -3.6305016212878627e-05 30 -3.6305016212878627e-05
		 31 -3.6305016212878627e-05 32 -3.6305016212878627e-05 33 -3.6305016212878627e-05
		 34 -3.6305016212878627e-05 35 -3.6305016212878627e-05 36 -3.6305016212878627e-05
		 37 -3.6305016212878627e-05 38 -3.6305016212878627e-05 39 -3.6305016212878627e-05
		 40 -3.6305016212878627e-05;
createNode animCurveTL -n "R_F_ankle_jnt_translateZ";
	rename -uid "AB1FE012-449C-AABB-A580-8A9652370BA7";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 3.7504031681123706e-05 1 3.7504031681123706e-05
		 2 3.7504031681123706e-05 3 3.7504031681123706e-05 4 3.7504031681123706e-05 5 3.7504031681123706e-05
		 6 3.7504031681123706e-05 7 3.7504031681123706e-05 8 3.7504031681123706e-05 9 3.7504031681123706e-05
		 10 3.7504031681123706e-05 11 3.7504031681123706e-05 12 3.7504031681123706e-05 13 3.7504031681123706e-05
		 14 3.7504031681123706e-05 15 3.7504031681123706e-05 16 3.7504031681123706e-05 17 3.7504031681123706e-05
		 18 3.7504031681123706e-05 19 3.7504031681123706e-05 20 3.7504031681123706e-05 21 3.7504031681123706e-05
		 22 3.7504031681123706e-05 23 3.7504031681123706e-05 24 3.7504031681123706e-05 25 3.7504031681123706e-05
		 26 3.7504031681123706e-05 27 3.7504031681123706e-05 28 3.7504031681123706e-05 29 3.7504031681123706e-05
		 30 3.7504031681123706e-05 31 3.7504031681123706e-05 32 3.7504031681123706e-05 33 3.7504031681123706e-05
		 34 3.7504031681123706e-05 35 3.7504031681123706e-05 36 3.7504031681123706e-05 37 3.7504031681123706e-05
		 38 3.7504031681123706e-05 39 3.7504031681123706e-05 40 3.7504031681123706e-05;
createNode animCurveTA -n "R_F_ankle_jnt_rotateX";
	rename -uid "9B87BF32-412A-82A5-8BA5-B181E26F631E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.7981564697105146 1 1.8572440578965237
		 2 2.0958356062083623 3 2.4949830907142809 4 2.9735127101768337 5 3.4503637549117618
		 6 3.7627313814797723 7 4.0354933573127809 8 4.5311863998879316 9 5.8311464944366866
		 10 7.7875120408370817 11 7.4981549768001061 12 5.1293548954816082 13 3.562299707699125
		 14 2.5148198492667277 15 1.8329649241653891 16 1.2398678204864249 17 0.71668225473526015
		 18 0.40161879113630655 19 0.3892102076625667 20 0.62895368034699262 21 0.93891049334894383
		 22 1.195812264344716 23 1.2936181182676676 24 1.2501354932789353 25 1.3997846479376324
		 26 1.9183524690419034 27 2.7022239650335838 28 3.6537520807401873 29 4.6768011936424685
		 30 5.6062716029675821 31 6.0814650668409183 32 4.933698796847179 33 2.8300884676542664
		 34 1.605909031155639 35 1.3982463223637551 36 1.2903691294490369 37 1.3251015175492229
		 38 1.491619659958475 39 1.6943354637947095 40 1.7981564697105146;
createNode animCurveTA -n "R_F_ankle_jnt_rotateY";
	rename -uid "F4D42AA4-4026-8118-3F59-7A83F3AA6235";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.19852744631162694 1 -0.094782410442893136
		 2 -0.62847975929004862 3 -1.0351416816429928 4 -1.5718433749755616 5 -1.8801374462026157
		 6 -1.321408211712169 7 -0.1859561642561143 8 1.3258276364318389 9 2.7818903766689593
		 10 3.5408704937957141 11 3.5402912197973886 12 2.8546506064245265 13 1.8611513067620569
		 14 0.94842878851248569 15 0.31958419187436971 16 -0.26411462298940097 17 -0.90172966327072557
		 18 -1.3544292072259747 19 -1.649032836348413 20 -1.8374772368418115 21 -1.8670973097448746
		 22 -1.7610212322042891 23 -1.5928535001023072 24 -1.456174368940935 25 -1.31539542901846
		 26 -1.0480293398991043 27 -0.60121216334445193 28 0.016334011993443868 29 0.74317278175981616
		 30 1.4520650448494254 31 1.8798939808144448 32 1.5102847325939546 33 0.67888073094483303
		 34 0.19437703475825768 35 0.07318053684985705 36 -0.10534940796028831 37 -0.18574116443477845
		 38 -0.080967563723451014 39 0.10168701940478814 40 0.19852744631162694;
createNode animCurveTA -n "R_F_ankle_jnt_rotateZ";
	rename -uid "BFA3CEC7-4476-B973-AB60-76A10EE21C0B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -0.046296501284201756 1 -0.93639894181124939
		 2 -2.516732744283884 3 -3.7036601917692336 4 -5.2169534423313237 5 -6.0958775981909383
		 6 -4.6577338611281256 7 -1.5117947814666481 8 3.5346660405708437 9 10.304129676319688
		 10 16.990730458176202 11 17.03766852899027 12 10.863686306455588 13 5.9321761035888319
		 14 2.4421215410118071 15 0.34442253285494201 16 -1.3188796909655265 17 -2.9168427354523092
		 18 -3.9167643511247827 19 -4.4502020155035797 20 -4.5943340433247721 21 -4.2749888175908612
		 22 -3.6589517809107135 23 -3.0458915252813399 24 -2.6832039166945374 25 -2.4734990052779775
		 26 -2.163245691800908 27 -1.5216442871952132 28 -0.3768253762165033 29 1.3005419246503609
		 30 3.3279539655666066 31 4.8483675594350668 32 3.7098932872130304 33 1.2968808857209411
		 34 0.056451080420666724 35 -0.25456491685533539 36 -0.74751733744666526 37 -1.0185433234696981
		 38 -0.8010279272383839 39 -0.32142587709356379 40 -0.04629650128420177;
createNode animCurveTU -n "R_F_ankle_jnt_scaleX";
	rename -uid "1ED4DCF2-4725-1B4E-5EF5-649950E0B4ED";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_F_ankle_jnt_scaleY";
	rename -uid "8A277DF4-4210-8E11-A5D6-D6AC6651BE18";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_F_ankle_jnt_scaleZ";
	rename -uid "68EC4853-41C1-2B53-44B3-B790A4F8BB63";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_F_elbow_jnt_visibility";
	rename -uid "8BC9F94F-4EA2-D04C-10AE-01A3549C990D";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "R_F_elbow_jnt_translateX";
	rename -uid "D4B06F12-4886-BE9B-27BB-1DADEE1A490B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -51.388376334145313 1 -51.388376334145313
		 2 -51.388376334145313 3 -51.388376334145313 4 -51.388376334145313 5 -51.388376334145313
		 6 -51.388376334145313 7 -51.388376334145313 8 -51.388376334145313 9 -51.388376334145313
		 10 -51.388376334145313 11 -51.388376334145313 12 -51.388376334145313 13 -51.388376334145313
		 14 -51.388376334145313 15 -51.388376334145313 16 -51.388376334145313 17 -51.388376334145313
		 18 -51.388376334145313 19 -51.388376334145313 20 -51.388376334145313 21 -51.388376334145313
		 22 -51.388376334145313 23 -51.388376334145313 24 -51.388376334145313 25 -51.388376334145313
		 26 -51.388376334145313 27 -51.388376334145313 28 -51.388376334145313 29 -51.388376334145313
		 30 -51.388376334145313 31 -51.388376334145313 32 -51.388376334145313 33 -51.388376334145313
		 34 -51.388376334145313 35 -51.388376334145313 36 -51.388376334145313 37 -51.388376334145313
		 38 -51.388376334145313 39 -51.388376334145313 40 -51.388376334145313;
createNode animCurveTL -n "R_F_elbow_jnt_translateY";
	rename -uid "5C727C9A-462D-4034-7439-93A8C47C1248";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.5523140849182937e-05 1 -1.5523140849182937e-05
		 2 -1.5523140849182937e-05 3 -1.5523140849182937e-05 4 -1.5523140849182937e-05 5 -1.5523140849182937e-05
		 6 -1.5523140849182937e-05 7 -1.5523140849182937e-05 8 -1.5523140849182937e-05 9 -1.5523140849182937e-05
		 10 -1.5523140849182937e-05 11 -1.5523140849182937e-05 12 -1.5523140849182937e-05
		 13 -1.5523140849182937e-05 14 -1.5523140849182937e-05 15 -1.5523140849182937e-05
		 16 -1.5523140849182937e-05 17 -1.5523140849182937e-05 18 -1.5523140849182937e-05
		 19 -1.5523140849182937e-05 20 -1.5523140849182937e-05 21 -1.5523140849182937e-05
		 22 -1.5523140849182937e-05 23 -1.5523140849182937e-05 24 -1.5523140849182937e-05
		 25 -1.5523140849182937e-05 26 -1.5523140849182937e-05 27 -1.5523140849182937e-05
		 28 -1.5523140849182937e-05 29 -1.5523140849182937e-05 30 -1.5523140849182937e-05
		 31 -1.5523140849182937e-05 32 -1.5523140849182937e-05 33 -1.5523140849182937e-05
		 34 -1.5523140849182937e-05 35 -1.5523140849182937e-05 36 -1.5523140849182937e-05
		 37 -1.5523140849182937e-05 38 -1.5523140849182937e-05 39 -1.5523140849182937e-05
		 40 -1.5523140849182937e-05;
createNode animCurveTL -n "R_F_elbow_jnt_translateZ";
	rename -uid "E80F7C81-474E-4D29-5D86-2B8E0D6739A8";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 3.323466174265377e-06 1 3.323466174265377e-06
		 2 3.323466174265377e-06 3 3.323466174265377e-06 4 3.323466174265377e-06 5 3.323466174265377e-06
		 6 3.323466174265377e-06 7 3.323466174265377e-06 8 3.323466174265377e-06 9 3.323466174265377e-06
		 10 3.323466174265377e-06 11 3.323466174265377e-06 12 3.323466174265377e-06 13 3.323466174265377e-06
		 14 3.323466174265377e-06 15 3.323466174265377e-06 16 3.323466174265377e-06 17 3.323466174265377e-06
		 18 3.323466174265377e-06 19 3.323466174265377e-06 20 3.323466174265377e-06 21 3.323466174265377e-06
		 22 3.323466174265377e-06 23 3.323466174265377e-06 24 3.323466174265377e-06 25 3.323466174265377e-06
		 26 3.323466174265377e-06 27 3.323466174265377e-06 28 3.323466174265377e-06 29 3.323466174265377e-06
		 30 3.323466174265377e-06 31 3.323466174265377e-06 32 3.323466174265377e-06 33 3.323466174265377e-06
		 34 3.323466174265377e-06 35 3.323466174265377e-06 36 3.323466174265377e-06 37 3.323466174265377e-06
		 38 3.323466174265377e-06 39 3.323466174265377e-06 40 3.323466174265377e-06;
createNode animCurveTA -n "R_F_elbow_jnt_rotateX";
	rename -uid "18EA0F28-4751-67DF-4BE7-C7BA1B60F8F3";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -8.9891610340481458e-19 1 9.2088299229520207e-07
		 2 2.5851953372795719e-06 3 3.8815173917106896e-06 4 5.513463651652335e-06 5 6.4122846834233733e-06
		 6 4.7959941844794515e-06 7 1.269407835492973e-06 8 -5.0723044956284709e-06 9 -1.3570292936776929e-05
		 10 -2.172923448388821e-05 11 -2.172923448388821e-05 12 -1.4061418183435691e-05 13 -8.0098394869200923e-06
		 14 -3.5302546568332836e-06 15 -5.3154405644430304e-07 16 2.3231448790731206e-06 17 5.459983363295452e-06
		 18 7.781432723847874e-06 19 9.6910444702572471e-06 20 1.1659205694450901e-05 21 1.3243317317793669e-05
		 22 1.4215016679828836e-05 23 1.4485334265747132e-05 24 1.4274351881104547e-05 25 1.3655386741411357e-05
		 26 1.2210901632288016e-05 27 9.7976654940508218e-06 28 6.5628768927046305e-06 29 2.795931018306018e-06
		 30 -9.9720386930344036e-07 31 -3.5718741368926578e-06 32 -2.1877782722103107e-06
		 33 1.0878638101529555e-06 34 2.8644866674919553e-06 35 3.324116187098388e-06 36 3.7930989837561331e-06
		 37 3.5142811700287361e-06 38 2.2499760500438456e-06 39 7.4133209824941994e-07 40 -8.9887380732055102e-19;
createNode animCurveTA -n "R_F_elbow_jnt_rotateY";
	rename -uid "3DDC33BB-4819-1E65-C122-3E953321CAE2";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -4.4336348642469639e-13 1 0.49259291250119164
		 2 1.382856357680263 3 2.0762767614388475 4 2.9492271448816827 5 3.4300188128371394
		 6 2.5654429101609328 7 0.67902361979649606 8 -2.5017625080599433 9 -6.6931411790988582
		 10 -10.717295108660322 11 -10.717295108660322 12 -6.9353740201894345 13 -3.9506137971852837
		 14 -1.7411925392060368 15 -0.2621682103155541 16 1.2426819820970427 17 2.9206198068989702
		 18 4.1623948329404676 19 5.183872284235103 20 6.2366686522960251 21 7.0840316341277729
		 22 7.603806918094592 23 7.7484034934053243 24 7.6355461291070554 25 7.3044532069343386
		 26 6.5317783579893893 27 5.2409053286976075 28 3.5105726460499302 29 1.4955817568387511
		 30 -0.49184098771391838 31 -1.761720103138215 32 -1.0790562084908433 33 0.58191323667763639
		 34 1.5322531115965823 35 1.7781152304854904 36 2.0289805452447585 37 1.8798370817751178
		 38 1.2035429743214723 39 0.39654868258250925 40 -4.4333950489587552e-13;
createNode animCurveTA -n "R_F_elbow_jnt_rotateZ";
	rename -uid "0179CB3F-492B-602F-4869-9A9FB9D986D6";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.1021202894158847e-12 1 1.1670434081167513
		 2 3.2762416097474545 3 4.9190823626747608 4 6.9872627297795891 5 8.1263468142740365
		 6 6.078007135752137 7 1.6087321179984679 8 -6.2189226308809742 9 -16.637921072155557
		 10 -26.64122948455373 11 -26.64122948455373 12 -17.240067475959879 13 -9.8205011347134263
		 14 -4.3282852196816428 15 -0.65170207442804062 16 2.944142676003898 17 6.9194866729805344
		 18 9.8614806029119375 19 12.281548971375157 20 14.775817626594153 21 16.783376722821515
		 22 18.014820179397883 23 18.357396119428767 24 18.090016220694629 25 17.305596058549384
		 26 15.474986916204834 27 12.416670766463174 28 8.3171936934387798 29 3.5433088587406587
		 30 -1.2226264641166524 31 -4.3793129777055961 32 -2.6823357746216314 33 1.3786597202794215
		 34 3.6301900576997164 35 4.2126827364879507 36 4.807028908519464 37 4.4536805523237692
		 38 2.8514151521895976 39 0.93949692385023875 40 -1.1020621971271085e-12;
createNode animCurveTU -n "R_F_elbow_jnt_scaleX";
	rename -uid "5BC5CB17-4C51-EADB-7C3E-43BA3324B4E1";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_F_elbow_jnt_scaleY";
	rename -uid "C4D51F01-419C-5CFD-C2B1-09AA35D8AC45";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_F_elbow_jnt_scaleZ";
	rename -uid "2A69C90C-439A-8A5B-3E05-9EA0304CD832";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_F_shoulder_jnt_visibility";
	rename -uid "5C97F7A6-48AF-2710-76E0-56B34D286D6C";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "R_F_shoulder_jnt_translateX";
	rename -uid "F66DCF31-44E9-4630-0AAA-F2A0D45CFD65";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -85.078622067837017 1 -85.078622067837017
		 2 -85.078622067837017 3 -85.078622067837017 4 -85.078622067837017 5 -85.078622067837017
		 6 -85.078622067837017 7 -85.078622067837017 8 -85.078622067837017 9 -85.078622067837017
		 10 -85.078622067837017 11 -85.078622067837017 12 -85.078622067837017 13 -85.078622067837017
		 14 -85.078622067837017 15 -85.078622067837017 16 -85.078622067837017 17 -85.078622067837017
		 18 -85.078622067837017 19 -85.078622067837017 20 -85.078622067837017 21 -85.078622067837017
		 22 -85.078622067837017 23 -85.078622067837017 24 -85.078622067837017 25 -85.078622067837017
		 26 -85.078622067837017 27 -85.078622067837017 28 -85.078622067837017 29 -85.078622067837017
		 30 -85.078622067837017 31 -85.078622067837017 32 -85.078622067837017 33 -85.078622067837017
		 34 -85.078622067837017 35 -85.078622067837017 36 -85.078622067837017 37 -85.078622067837017
		 38 -85.078622067837017 39 -85.078622067837017 40 -85.078622067837017;
createNode animCurveTL -n "R_F_shoulder_jnt_translateY";
	rename -uid "B0C4B0E9-4EF2-387E-6611-8E8F231D6087";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 7.0848811482449037e-05 1 7.0848811482449037e-05
		 2 7.0848811482449037e-05 3 7.0848811482449037e-05 4 7.0848811482449037e-05 5 7.0848811482449037e-05
		 6 7.0848811482449037e-05 7 7.0848811482449037e-05 8 7.0848811482449037e-05 9 7.0848811482449037e-05
		 10 7.0848811482449037e-05 11 7.0848811482449037e-05 12 7.0848811482449037e-05 13 7.0848811482449037e-05
		 14 7.0848811482449037e-05 15 7.0848811482449037e-05 16 7.0848811482449037e-05 17 7.0848811482449037e-05
		 18 7.0848811482449037e-05 19 7.0848811482449037e-05 20 7.0848811482449037e-05 21 7.0848811482449037e-05
		 22 7.0848811482449037e-05 23 7.0848811482449037e-05 24 7.0848811482449037e-05 25 7.0848811482449037e-05
		 26 7.0848811482449037e-05 27 7.0848811482449037e-05 28 7.0848811482449037e-05 29 7.0848811482449037e-05
		 30 7.0848811482449037e-05 31 7.0848811482449037e-05 32 7.0848811482449037e-05 33 7.0848811482449037e-05
		 34 7.0848811482449037e-05 35 7.0848811482449037e-05 36 7.0848811482449037e-05 37 7.0848811482449037e-05
		 38 7.0848811482449037e-05 39 7.0848811482449037e-05 40 7.0848811482449037e-05;
createNode animCurveTL -n "R_F_shoulder_jnt_translateZ";
	rename -uid "4E09C4E7-42C0-F7F0-540D-62A01D9E9B07";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.00015208840052594041 1 0.00015208840052594041
		 2 0.00015208840052594041 3 0.00015208840052594041 4 0.00015208840052594041 5 0.00015208840052594041
		 6 0.00015208840052594041 7 0.00015208840052594041 8 0.00015208840052594041 9 0.00015208840052594041
		 10 0.00015208840052594041 11 0.00015208840052594041 12 0.00015208840052594041 13 0.00015208840052594041
		 14 0.00015208840052594041 15 0.00015208840052594041 16 0.00015208840052594041 17 0.00015208840052594041
		 18 0.00015208840052594041 19 0.00015208840052594041 20 0.00015208840052594041 21 0.00015208840052594041
		 22 0.00015208840052594041 23 0.00015208840052594041 24 0.00015208840052594041 25 0.00015208840052594041
		 26 0.00015208840052594041 27 0.00015208840052594041 28 0.00015208840052594041 29 0.00015208840052594041
		 30 0.00015208840052594041 31 0.00015208840052594041 32 0.00015208840052594041 33 0.00015208840052594041
		 34 0.00015208840052594041 35 0.00015208840052594041 36 0.00015208840052594041 37 0.00015208840052594041
		 38 0.00015208840052594041 39 0.00015208840052594041 40 0.00015208840052594041;
createNode animCurveTA -n "R_F_shoulder_jnt_rotateX";
	rename -uid "F925982B-48C4-1027-5522-70BD64AB137E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.2608988734093125 1 0.25839884773474286
		 2 0.18823066754053 3 0.032899175273292912 4 -0.11313734972713752 5 -0.21016895245325273
		 6 -0.269423853004499 7 -0.28598202151958607 8 -0.17454309260591389 9 0.15653931201708851
		 10 0.61436549105693927 11 0.81884370244177351 12 0.7170721448950661 13 0.62606610089616843
		 14 0.50085867986497945 15 0.28268955447226524 16 -0.23751641366667448 17 -0.98639899809402953
		 18 -1.5823900409197198 19 -2.0197576038037814 20 -2.4816314743894061 21 -2.8977496572384425
		 22 -3.2525275265326496 23 -3.5635302909609199 24 -3.8125062283829037 25 -4.0378616066867652
		 26 -4.1796065291792663 27 -4.1616815236179558 28 -4.0446148779556248 29 -3.8861292254622231
		 30 -3.7665132268410595 31 -3.7452655304984184 32 -3.3698919620610215 33 -2.6296457341803943
		 34 -2.1996222947373325 35 -2.1230730222776741 36 -1.9700539253213429 37 -1.5228066827722848
		 38 -0.78059821360458437 39 -0.064276202491325804 40 0.2608988734093125;
createNode animCurveTA -n "R_F_shoulder_jnt_rotateY";
	rename -uid "546B7530-49BA-61AF-B222-AF860E229C16";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.032251228778106114 1 -0.16156608637797462
		 2 -0.68346832376876232 3 -1.3698525703398126 4 -2.1808552436042308 5 -2.7367912508540013
		 6 -2.6945820432278977 7 -2.1639837796911867 8 -0.30163475694323294 9 2.6324215338282446
		 10 5.5239740123017018 11 5.8269232754240674 12 3.6607082801977109 13 2.0975980712600073
		 14 1.0139368849824841 15 0.27654011813218798 16 -0.6160903286996795 17 -1.7803958650602847
		 18 -2.7941595453448542 19 -4.0642599349884625 20 -5.9244342825389271 21 -7.9052149403816019
		 22 -9.5443321006472086 23 -10.412936650361134 24 -10.489120965420252 25 -10.123716382783408
		 26 -9.2957312688381801 27 -7.9998137085341199 28 -6.4021578798567313 29 -4.6860566996792246
		 30 -3.1682448066039344 31 -2.3027567292636926 32 -2.4683679963755445 33 -2.9549911446504482
		 34 -3.2713809104098477 35 -3.4562916591265358 36 -3.3509100275971124 37 -2.6806558980726147
		 38 -1.5478719579799296 39 -0.45822679541424866 40 0.0322512287781061;
createNode animCurveTA -n "R_F_shoulder_jnt_rotateZ";
	rename -uid "2381FB43-44C9-97F7-DEE7-81B47EC90A51";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -0.032943331800422006 1 -0.047990610853366783
		 2 -0.10233074093674162 3 -0.17208979547397901 4 -0.42727141484965703 5 -0.47583763208428886
		 6 0.039956028260010025 7 0.87530091924372988 8 2.7354192049896882 9 5.3958049328698063
		 10 7.8609141372801146 11 7.8014676466539203 12 5.3555637753382435 13 3.3328491336094705
		 14 1.6505495122938927 15 0.29496126984270798 16 -1.3316248708817107 17 -3.2879314834930602
		 18 -4.8410171888754441 19 -6.1831533766078852 20 -7.7254255335767716 21 -9.2215879874240887
		 22 -10.384706192928917 23 -10.966457818253096 24 -10.983801727910045 25 -10.373163359450157
		 26 -8.8722514257845297 27 -6.6213802538412461 28 -3.9700754218732683 29 -1.2998946089569396
		 30 0.87026375449255178 31 1.8857854907148903 32 0.54007316846855047 33 -2.0894006011844697
		 34 -3.7612859140553572 35 -4.2012845736652471 36 -4.2458575454454079 37 -3.5208701383119618
		 38 -2.1097104502731328 39 -0.68631607055799793 40 -0.032943331800421957;
createNode animCurveTU -n "R_F_shoulder_jnt_scaleX";
	rename -uid "F1F75F2F-4DC0-E016-BFA9-93A87B043D54";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_F_shoulder_jnt_scaleY";
	rename -uid "6B1654D9-4A68-DB75-CBCF-17B0E3C71716";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_F_shoulder_jnt_scaleZ";
	rename -uid "C1430CFF-4659-2ACC-AEE1-D5A3E443AA75";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_F_toe_jnt_visibility";
	rename -uid "C9AF5CD2-4FF1-19FB-5DBB-85B71E2B031E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "R_F_toe_jnt_translateX";
	rename -uid "F1E8F189-4408-7EDA-DD53-1CA472379B6A";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -25.232676349202073 1 -25.232676349202073
		 2 -25.232676349202073 3 -25.232676349202073 4 -25.232676349202073 5 -25.232676349202073
		 6 -25.232676349202073 7 -25.232676349202073 8 -25.232676349202073 9 -25.232676349202073
		 10 -25.232676349202073 11 -25.232676349202073 12 -25.232676349202073 13 -25.232676349202073
		 14 -25.232676349202073 15 -25.232676349202073 16 -25.232676349202073 17 -25.232676349202073
		 18 -25.232676349202073 19 -25.232676349202073 20 -25.232676349202073 21 -25.232676349202073
		 22 -25.232676349202073 23 -25.232676349202073 24 -25.232676349202073 25 -25.232676349202073
		 26 -25.232676349202073 27 -25.232676349202073 28 -25.232676349202073 29 -25.232676349202073
		 30 -25.232676349202073 31 -25.232676349202073 32 -25.232676349202073 33 -25.232676349202073
		 34 -25.232676349202073 35 -25.232676349202073 36 -25.232676349202073 37 -25.232676349202073
		 38 -25.232676349202073 39 -25.232676349202073 40 -25.232676349202073;
createNode animCurveTL -n "R_F_toe_jnt_translateY";
	rename -uid "88FF5977-4F68-2D11-7F54-2186C228E303";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 6.3773927561072696e-06 1 6.3773927561072696e-06
		 2 6.3773927561072696e-06 3 6.3773927561072696e-06 4 6.3773927561072696e-06 5 6.3773927561072696e-06
		 6 6.3773927561072696e-06 7 6.3773927561072696e-06 8 6.3773927561072696e-06 9 6.3773927561072696e-06
		 10 6.3773927561072696e-06 11 6.3773927561072696e-06 12 6.3773927561072696e-06 13 6.3773927561072696e-06
		 14 6.3773927561072696e-06 15 6.3773927561072696e-06 16 6.3773927561072696e-06 17 6.3773927561072696e-06
		 18 6.3773927561072696e-06 19 6.3773927561072696e-06 20 6.3773927561072696e-06 21 6.3773927561072696e-06
		 22 6.3773927561072696e-06 23 6.3773927561072696e-06 24 6.3773927561072696e-06 25 6.3773927561072696e-06
		 26 6.3773927561072696e-06 27 6.3773927561072696e-06 28 6.3773927561072696e-06 29 6.3773927561072696e-06
		 30 6.3773927561072696e-06 31 6.3773927561072696e-06 32 6.3773927561072696e-06 33 6.3773927561072696e-06
		 34 6.3773927561072696e-06 35 6.3773927561072696e-06 36 6.3773927561072696e-06 37 6.3773927561072696e-06
		 38 6.3773927561072696e-06 39 6.3773927561072696e-06 40 6.3773927561072696e-06;
createNode animCurveTL -n "R_F_toe_jnt_translateZ";
	rename -uid "C7A2E146-4D68-DC31-BA77-EDBC377F5926";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -5.8069600925136911e-05 1 -5.8069600925136911e-05
		 2 -5.8069600925136911e-05 3 -5.8069600925136911e-05 4 -5.8069600925136911e-05 5 -5.8069600925136911e-05
		 6 -5.8069600925136911e-05 7 -5.8069600925136911e-05 8 -5.8069600925136911e-05 9 -5.8069600925136911e-05
		 10 -5.8069600925136911e-05 11 -5.8069600925136911e-05 12 -5.8069600925136911e-05
		 13 -5.8069600925136911e-05 14 -5.8069600925136911e-05 15 -5.8069600925136911e-05
		 16 -5.8069600925136911e-05 17 -5.8069600925136911e-05 18 -5.8069600925136911e-05
		 19 -5.8069600925136911e-05 20 -5.8069600925136911e-05 21 -5.8069600925136911e-05
		 22 -5.8069600925136911e-05 23 -5.8069600925136911e-05 24 -5.8069600925136911e-05
		 25 -5.8069600925136911e-05 26 -5.8069600925136911e-05 27 -5.8069600925136911e-05
		 28 -5.8069600925136911e-05 29 -5.8069600925136911e-05 30 -5.8069600925136911e-05
		 31 -5.8069600925136911e-05 32 -5.8069600925136911e-05 33 -5.8069600925136911e-05
		 34 -5.8069600925136911e-05 35 -5.8069600925136911e-05 36 -5.8069600925136911e-05
		 37 -5.8069600925136911e-05 38 -5.8069600925136911e-05 39 -5.8069600925136911e-05
		 40 -5.8069600925136911e-05;
createNode animCurveTA -n "R_F_toe_jnt_rotateX";
	rename -uid "E53C3B9B-4307-18D3-5D5D-6FBE1E6A441E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_F_toe_jnt_rotateY";
	rename -uid "CC1441B0-44D1-ABF0-3BCD-D3B6B4D680D3";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_F_toe_jnt_rotateZ";
	rename -uid "1C5A35FA-4C1F-B8EB-F145-0D99CDE85CD3";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "R_F_toe_jnt_scaleX";
	rename -uid "82EE2841-426C-9A76-2844-C9BFC2942327";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_F_toe_jnt_scaleY";
	rename -uid "1951037F-42E6-CD72-8D80-E7B1C76B04B6";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_F_toe_jnt_scaleZ";
	rename -uid "AF27BF6C-4487-5E5B-FA2D-EB9F08E25AB8";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_brow_jnt_visibility";
	rename -uid "3F078879-4C7F-8735-97DD-F4BB8557B072";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "R_brow_jnt_translateX";
	rename -uid "D7D74EFD-4DD2-34F5-1ECC-05B90C57F130";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 17.677620421151005 1 17.677620916055087
		 2 17.677620421150991 3 17.677620421150991 4 17.677620421150959 5 17.677620421150973
		 6 17.677620421151026 7 17.677620421151012 8 17.677620421150987 9 17.67762042115098
		 10 17.677620421150966 11 17.677620421150998 12 17.677620421151001 13 17.677620421151005
		 14 17.677620421151001 15 17.677620421151019 16 17.677620421150991 17 17.677620783579687
		 18 17.677620421150973 19 17.677620421150998 20 17.677620421150976 21 17.677620421150998
		 22 17.677620421150955 23 17.677620421150994 24 17.677620421151005 25 17.677620421151005
		 26 17.677620421150976 27 17.677620421151012 28 17.677620421151012 29 17.677620421151012
		 30 17.677620421150969 31 17.677620421150969 32 17.677620421151005 33 17.677620421150994
		 34 17.677620421150973 35 17.677620421150969 36 17.677620421150969 37 17.677620421151019
		 38 17.677620421151012 39 17.677620421151005 40 17.677620421150998;
createNode animCurveTL -n "R_brow_jnt_translateY";
	rename -uid "4A662637-48B1-A675-201E-F79C47F2BCA8";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 40.936021990622748 1 40.936021776869126
		 2 40.936021990622734 3 40.936021990622805 4 40.936021990622734 5 40.93602199062282
		 6 40.936021990622763 7 40.936021990622791 8 40.936021990622763 9 40.936021990622763
		 10 40.936021990622748 11 40.936021990622805 12 40.936021990622763 13 40.936021990622777
		 14 40.936021990622791 15 40.936021990622748 16 40.936021990622777 17 40.93602183413617
		 18 40.936021990622763 19 40.936021990622748 20 40.936021990622763 21 40.936021990622791
		 22 40.936021990622777 23 40.936021990622777 24 40.936021990622748 25 40.936021990622748
		 26 40.936021990622748 27 40.936021990622748 28 40.936021990622791 29 40.936021990622777
		 30 40.936021990622791 31 40.936021990622777 32 40.936021990622748 33 40.936021990622763
		 34 40.936021990622763 35 40.936021990622805 36 40.936021990622763 37 40.936021990622791
		 38 40.936021990622748 39 40.936021990622763 40 40.936021990622805;
createNode animCurveTL -n "R_brow_jnt_translateZ";
	rename -uid "40F5953A-4515-70CF-F3BE-99A852E5CB7D";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 16.754700000000014 1 16.754700000089294
		 2 16.754700000000007 3 16.754700000000007 4 16.754700000000007 5 16.75470000000001
		 6 16.754700000000003 7 16.754700000000014 8 16.7547 9 16.7547 10 16.754700000000007
		 11 16.754700000000007 12 16.754699999999985 13 16.7547 14 16.754699999999993 15 16.754700000000003
		 16 16.75470000000001 17 16.754699999944041 18 16.754699999999993 19 16.7547 20 16.754700000000007
		 21 16.754700000000007 22 16.7547 23 16.75470000000001 24 16.7547 25 16.754700000000003
		 26 16.7547 27 16.754700000000007 28 16.754700000000007 29 16.754700000000007 30 16.754700000000007
		 31 16.7547 32 16.754700000000007 33 16.754700000000003 34 16.754700000000003 35 16.754700000000003
		 36 16.754700000000007 37 16.754700000000003 38 16.754700000000007 39 16.75470000000001
		 40 16.754700000000014;
createNode animCurveTA -n "R_brow_jnt_rotateX";
	rename -uid "0D9B8A11-4D75-32D9-6CD4-4B98745F8C4F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_brow_jnt_rotateY";
	rename -uid "DAC39175-4FFC-EA04-2069-14B08F93FA12";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_brow_jnt_rotateZ";
	rename -uid "72EFD1A9-4B8E-5101-7B86-2891D8772767";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 3.9477856048777889e-14 1 -6.9265827241881776e-07
		 2 6.4922293544110776e-14 3 3.9477856048777889e-14 4 6.4922293500486023e-14 5 6.4922293500486023e-14
		 6 3.9477856048777889e-14 7 3.9477856048777889e-14 8 6.4922293500486023e-14 9 3.9477856048777889e-14
		 10 6.4922293500486023e-14 11 3.9477856048777889e-14 12 6.4922293500486023e-14 13 3.9477856048777889e-14
		 14 3.9477856048777889e-14 15 3.9477856048777889e-14 16 3.9477856048777889e-14 17 -5.0728936258482326e-07
		 18 6.4922293544110776e-14 19 6.4922293500486023e-14 20 6.4922293500486023e-14 21 6.4922293500486023e-14
		 22 6.4922293500486023e-14 23 6.4922293500486023e-14 24 3.9477856048777889e-14 25 6.4922293500486023e-14
		 26 1.4033418597069755e-14 27 6.4922293500486023e-14 28 3.9477856048777889e-14 29 6.4922293500486023e-14
		 30 6.4922293500486023e-14 31 6.4922293500486023e-14 32 3.9477856048777889e-14 33 6.4922293500486023e-14
		 34 3.9477856048777889e-14 35 3.9477856048777889e-14 36 3.9477856048777889e-14 37 3.9477856048777889e-14
		 38 1.4033418597069755e-14 39 6.4922293500486023e-14 40 3.9477856048777889e-14;
createNode animCurveTU -n "R_brow_jnt_scaleX";
	rename -uid "6DBED4B2-486D-517A-301D-A4A4B821B15D";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_brow_jnt_scaleY";
	rename -uid "FE1D96EE-49C8-058B-1B79-D4B8A3E0E557";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_brow_jnt_scaleZ";
	rename -uid "817268F3-4D52-C011-0E40-6494507B91D5";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_cheek_jnt_visibility";
	rename -uid "70A50132-4ACB-DD5A-5856-5CAB044E9CF4";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "R_cheek_jnt_translateX";
	rename -uid "EAB6B26D-405D-11E8-9C83-B0BDBA450455";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 22.738264475745339 1 22.73826460204236
		 2 22.738264475745353 3 22.73826447574536 4 22.73826447574535 5 22.738264475745357
		 6 22.738264475745375 7 22.738264475745382 8 22.738264475745353 9 22.738264475745357
		 10 22.738264475745357 11 22.738264475745368 12 22.738264475745385 13 22.738264475745368
		 14 22.738264475745371 15 22.738264475745389 16 22.738264475745368 17 22.738264568166652
		 18 22.738264475745357 19 22.738264475745375 20 22.738264475745368 21 22.738264475745368
		 22 22.738264475745346 23 22.738264475745364 24 22.73826447574536 25 22.738264475745375
		 26 22.738264475745353 27 22.738264475745382 28 22.738264475745375 29 22.738264475745375
		 30 22.738264475745368 31 22.73826447574536 32 22.738264475745375 33 22.738264475745378
		 34 22.73826447574535 35 22.738264475745339 36 22.738264475745346 37 22.738264475745375
		 38 22.738264475745368 39 22.738264475745375 40 22.73826447574536;
createNode animCurveTL -n "R_cheek_jnt_translateY";
	rename -uid "A4179D1C-4F6D-5A1D-AB47-CAB58E1BD9CF";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 10.44248499927491 1 10.442484724269235
		 2 10.442484999274896 3 10.442484999274939 4 10.442484999274882 5 10.442484999274924
		 6 10.442484999274896 7 10.442484999274896 8 10.442484999274896 9 10.442484999274896
		 10 10.442484999274882 11 10.442484999274939 12 10.442484999274868 13 10.44248499927491
		 14 10.442484999274896 15 10.44248499927491 16 10.44248499927491 17 10.442484798027806
		 18 10.442484999274924 19 10.442484999274882 20 10.442484999274896 21 10.442484999274924
		 22 10.44248499927491 23 10.44248499927491 24 10.44248499927491 25 10.442484999274882
		 26 10.442484999274882 27 10.44248499927491 28 10.442484999274896 29 10.442484999274939
		 30 10.442484999274896 31 10.442484999274939 32 10.442484999274882 33 10.442484999274924
		 34 10.442484999274924 35 10.442484999274939 36 10.442484999274896 37 10.442484999274924
		 38 10.44248499927491 39 10.442484999274924 40 10.44248499927491;
createNode animCurveTL -n "R_cheek_jnt_translateZ";
	rename -uid "F1FFB043-41C8-1FB0-C6B1-889BF6B72AFB";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 43.436500000000017 1 43.436499999999256
		 2 43.436500000000009 3 43.4365 4 43.436499999999995 5 43.4365 6 43.436500000000009
		 7 43.4365 8 43.436499999999981 9 43.436499999999988 10 43.4365 11 43.436499999999995
		 12 43.436499999999967 13 43.436499999999995 14 43.436499999999988 15 43.4365 16 43.436499999999995
		 17 43.436500000000464 18 43.436499999999967 19 43.436499999999981 20 43.436499999999995
		 21 43.4365 22 43.436499999999988 23 43.436500000000017 24 43.436499999999974 25 43.436499999999995
		 26 43.436499999999981 27 43.436500000000009 28 43.436499999999995 29 43.436500000000009
		 30 43.436499999999995 31 43.436499999999988 32 43.4365 33 43.4365 34 43.436499999999988
		 35 43.436500000000009 36 43.436499999999995 37 43.436499999999995 38 43.436500000000009
		 39 43.436500000000009 40 43.436500000000009;
createNode animCurveTA -n "R_cheek_jnt_rotateX";
	rename -uid "8CCF4C3E-43B3-9599-5509-82B3676974AC";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_cheek_jnt_rotateY";
	rename -uid "F6111A80-4624-1D3F-B9F8-D6A9C2B8387C";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_cheek_jnt_rotateZ";
	rename -uid "535F4D12-4F8C-826C-0EE2-1C9FACCB659F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 3.9477856048777889e-14 1 -6.9265827241881776e-07
		 2 6.4922293544110776e-14 3 3.9477856048777889e-14 4 6.4922293500486023e-14 5 6.4922293500486023e-14
		 6 3.9477856048777889e-14 7 3.9477856048777889e-14 8 6.4922293500486023e-14 9 3.9477856048777889e-14
		 10 6.4922293500486023e-14 11 3.9477856048777889e-14 12 6.4922293500486023e-14 13 3.9477856048777889e-14
		 14 3.9477856048777889e-14 15 3.9477856048777889e-14 16 3.9477856048777889e-14 17 -5.0728936258482326e-07
		 18 6.4922293544110776e-14 19 6.4922293500486023e-14 20 6.4922293500486023e-14 21 6.4922293500486023e-14
		 22 6.4922293500486023e-14 23 6.4922293500486023e-14 24 3.9477856048777889e-14 25 6.4922293500486023e-14
		 26 1.4033418597069755e-14 27 6.4922293500486023e-14 28 3.9477856048777889e-14 29 6.4922293500486023e-14
		 30 6.4922293500486023e-14 31 6.4922293500486023e-14 32 3.9477856048777889e-14 33 6.4922293500486023e-14
		 34 3.9477856048777889e-14 35 3.9477856048777889e-14 36 3.9477856048777889e-14 37 3.9477856048777889e-14
		 38 1.4033418597069755e-14 39 6.4922293500486023e-14 40 3.9477856048777889e-14;
createNode animCurveTU -n "R_cheek_jnt_scaleX";
	rename -uid "ADFA4C0F-4228-6BB1-FF4A-A1B23A0107E0";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 0.99999999999999989 2 1 3 1.0000000000000002
		 4 0.99999999999999989 5 1 6 1 7 1.0000000000000002 8 0.99999999999999956 9 0.99999999999999967
		 10 0.99999999999999967 11 0.99999999999999989 12 0.99999999999999956 13 0.99999999999999989
		 14 0.99999999999999944 15 1 16 1.0000000000000002 17 1 18 0.99999999999999956 19 0.99999999999999944
		 20 0.99999999999999989 21 1 22 0.99999999999999944 23 1.0000000000000002 24 0.99999999999999956
		 25 1 26 0.99999999999999956 27 1 28 0.99999999999999967 29 1 30 1 31 0.99999999999999967
		 32 1 33 0.99999999999999978 34 0.99999999999999989 35 0.99999999999999978 36 0.99999999999999978
		 37 1 38 1.0000000000000002 39 1 40 1;
createNode animCurveTU -n "R_cheek_jnt_scaleY";
	rename -uid "FCEEC7DA-49E7-7A8B-B008-208B1E60BA1E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.99999999999999978 1 0.99999999999999967
		 2 0.99999999999999978 3 1.0000000000000002 4 0.99999999999999989 5 1 6 1 7 1 8 0.99999999999999978
		 9 1.0000000000000002 10 0.99999999999999978 11 1 12 0.99999999999999989 13 1 14 0.99999999999999978
		 15 1.0000000000000002 16 1.0000000000000002 17 1 18 0.99999999999999989 19 0.99999999999999967
		 20 1 21 1 22 1 23 1 24 1 25 0.99999999999999989 26 0.99999999999999989 27 1 28 1.0000000000000002
		 29 1 30 1 31 1 32 1 33 0.99999999999999978 34 1 35 1 36 1 37 1.0000000000000002 38 1
		 39 0.99999999999999989 40 1;
createNode animCurveTU -n "R_cheek_jnt_scaleZ";
	rename -uid "ACD4EC3C-4394-618C-7A0C-F19A904BAFBD";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 0.99999999999999922 2 0.99999999999999978
		 3 0.99999999999999978 4 0.99999999999999933 5 1.0000000000000002 6 1 7 0.99999999999999978
		 8 0.99999999999999944 9 0.99999999999999933 10 0.99999999999999944 11 0.99999999999999978
		 12 0.99999999999999956 13 1 14 0.99999999999999944 15 1 16 0.99999999999999956 17 0.99999999999999933
		 18 0.99999999999999944 19 0.99999999999999922 20 0.99999999999999978 21 0.99999999999999978
		 22 0.99999999999999944 23 1 24 0.99999999999999956 25 0.99999999999999978 26 0.99999999999999911
		 27 0.99999999999999956 28 0.99999999999999956 29 0.99999999999999978 30 1 31 0.99999999999999944
		 32 0.99999999999999967 33 0.99999999999999978 34 0.999999999999999 35 0.99999999999999933
		 36 0.99999999999999944 37 0.99999999999999956 38 0.99999999999999978 39 1 40 1;
createNode animCurveTU -n "R_lipcorner_jnt_visibility";
	rename -uid "D3FD751E-4402-8291-AAF5-9185C2785DDD";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "R_lipcorner_jnt_translateX";
	rename -uid "D198ABF0-4719-117E-226E-F6BACBF61978";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 39.373344762218949 1 39.373344808145262
		 2 39.373344762218963 3 39.373344762218956 4 39.373344762218963 5 39.373344762218963
		 6 39.373344762218977 7 39.373344762218984 8 39.373344762218949 9 39.373344762218949
		 10 39.373344762218963 11 39.373344762218949 12 39.373344762218984 13 39.37334476221897
		 14 39.373344762218949 15 39.373344762218977 16 39.37334476221897 17 39.373344795787929
		 18 39.373344762218956 19 39.37334476221897 20 39.373344762218956 21 39.37334476221897
		 22 39.373344762218942 23 39.373344762218963 24 39.373344762218956 25 39.373344762218977
		 26 39.373344762218942 27 39.373344762218977 28 39.373344762218963 29 39.373344762218991
		 30 39.373344762218963 31 39.37334476221897 32 39.373344762218963 33 39.373344762218991
		 34 39.373344762218949 35 39.373344762218956 36 39.373344762218963 37 39.373344762218963
		 38 39.373344762218963 39 39.37334476221897 40 39.373344762218942;
createNode animCurveTL -n "R_lipcorner_jnt_translateY";
	rename -uid "8A6CC390-4DA6-0282-5FF1-80AB6CB65216";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 3.7949134319596851 1 3.7949129558647599
		 2 3.7949134319596709 3 3.794913431959742 4 3.7949134319596851 5 3.7949134319597277
		 6 3.7949134319596993 7 3.7949134319596993 8 3.7949134319596993 9 3.7949134319596993
		 10 3.7949134319596993 11 3.7949134319597135 12 3.7949134319596709 13 3.7949134319597135
		 14 3.7949134319596993 15 3.7949134319596851 16 3.7949134319596851 17 3.7949130834183222
		 18 3.7949134319597277 19 3.7949134319596851 20 3.7949134319597135 21 3.7949134319597135
		 22 3.7949134319597135 23 3.7949134319596851 24 3.7949134319596709 25 3.7949134319596709
		 26 3.7949134319596851 27 3.7949134319596851 28 3.7949134319596851 29 3.7949134319597135
		 30 3.7949134319596993 31 3.7949134319597135 32 3.7949134319596851 33 3.7949134319597135
		 34 3.7949134319596993 35 3.7949134319597135 36 3.7949134319596993 37 3.7949134319596993
		 38 3.7949134319596851 39 3.7949134319596993 40 3.7949134319597135;
createNode animCurveTL -n "R_lipcorner_jnt_translateZ";
	rename -uid "3E04B1C3-4005-2B67-AC47-CF8DBFD039A6";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 37.943800000000024 1 37.943799999959595
		 2 37.94380000000001 3 37.943800000000017 4 37.9438 5 37.94380000000001 6 37.94380000000001
		 7 37.943800000000017 8 37.943799999999989 9 37.943799999999989 10 37.9438 11 37.94380000000001
		 12 37.943799999999975 13 37.9438 14 37.9438 15 37.943800000000017 16 37.94380000000001
		 17 37.943800000025334 18 37.943799999999989 19 37.943799999999996 20 37.94380000000001
		 21 37.9438 22 37.94380000000001 23 37.943800000000017 24 37.943799999999989 25 37.94380000000001
		 26 37.943799999999996 27 37.943800000000024 28 37.9438 29 37.94380000000001 30 37.943800000000017
		 31 37.943799999999996 32 37.9438 33 37.9438 34 37.9438 35 37.943799999999996 36 37.94380000000001
		 37 37.94380000000001 38 37.94380000000001 39 37.943800000000017 40 37.943800000000024;
createNode animCurveTA -n "R_lipcorner_jnt_rotateX";
	rename -uid "8BE462DD-4676-6E02-81EF-31996F7751AE";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_lipcorner_jnt_rotateY";
	rename -uid "947AAE39-4FC3-9E5C-5458-DC84BF0E3F49";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_lipcorner_jnt_rotateZ";
	rename -uid "26D1814F-43C9-BAE1-8544-03AE4CBB4387";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 3.9477856048777889e-14 1 -6.9265827241881776e-07
		 2 6.4922293544110776e-14 3 3.9477856048777889e-14 4 6.4922293500486023e-14 5 6.4922293500486023e-14
		 6 3.9477856048777889e-14 7 3.9477856048777889e-14 8 6.4922293500486023e-14 9 3.9477856048777889e-14
		 10 6.4922293500486023e-14 11 3.9477856048777889e-14 12 6.4922293500486023e-14 13 3.9477856048777889e-14
		 14 3.9477856048777889e-14 15 3.9477856048777889e-14 16 3.9477856048777889e-14 17 -5.0728936258482326e-07
		 18 6.4922293544110776e-14 19 6.4922293500486023e-14 20 6.4922293500486023e-14 21 6.4922293500486023e-14
		 22 6.4922293500486023e-14 23 6.4922293500486023e-14 24 3.9477856048777889e-14 25 6.4922293500486023e-14
		 26 1.4033418597069755e-14 27 6.4922293500486023e-14 28 3.9477856048777889e-14 29 6.4922293500486023e-14
		 30 6.4922293500486023e-14 31 6.4922293500486023e-14 32 3.9477856048777889e-14 33 6.4922293500486023e-14
		 34 3.9477856048777889e-14 35 3.9477856048777889e-14 36 3.9477856048777889e-14 37 3.9477856048777889e-14
		 38 1.4033418597069755e-14 39 6.4922293500486023e-14 40 3.9477856048777889e-14;
createNode animCurveTU -n "R_lipcorner_jnt_scaleX";
	rename -uid "126C3D28-47EE-7723-310C-0F8061419CC7";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_lipcorner_jnt_scaleY";
	rename -uid "187C1808-4693-C2DD-FE82-FF9F2EF800DC";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_lipcorner_jnt_scaleZ";
	rename -uid "A86E7F31-42BF-F3FA-5C8B-4185D91BDB72";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_lowerlip1_jnt_visibility";
	rename -uid "E4DB3222-46B3-43F8-61AB-40AB4650B88C";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "R_lowerlip1_jnt_translateX";
	rename -uid "FC3AF4DD-4957-A7DD-407F-3A9BB2AF6BCD";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -22.725762052482693 1 -22.725762052465356
		 2 -22.725762052482693 3 -22.725762052482679 4 -22.725762052482708 5 -22.725762052482708
		 6 -22.725762052482679 7 -22.725762052482608 8 -22.725762052482665 9 -22.725762052482622
		 10 -22.725762052482665 11 -22.725762052482693 12 -22.725762052482679 13 -22.725762052482679
		 14 -22.725762052482636 15 -22.725762052482679 16 -22.725762052482651 17 -22.725762052497501
		 18 -22.725762052482636 19 -22.725762052482708 20 -22.725762052482708 21 -22.725762052482679
		 22 -22.725762052482651 23 -22.725762052482665 24 -22.725762052482665 25 -22.725762052482651
		 26 -22.725762052482622 27 -22.725762052482651 28 -22.725762052482665 29 -22.725762052482708
		 30 -22.725762052482679 31 -22.725762052482651 32 -22.725762052482651 33 -22.725762052482679
		 34 -22.725762052482665 35 -22.725762052482636 36 -22.725762052482651 37 -22.725762052482622
		 38 -22.725762052482679 39 -22.725762052482722 40 -22.725762052482665;
createNode animCurveTL -n "R_lowerlip1_jnt_translateY";
	rename -uid "9602CD4E-48B4-6651-9752-6393D3DF7B1C";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -43.185532492509864 1 -43.185532492574325
		 2 -43.185532492509836 3 -43.185532492509878 4 -43.185532492509822 5 -43.18553249250985
		 6 -43.185532492509907 7 -43.185532492509843 8 -43.185532492509857 9 -43.185532492509864
		 10 -43.18553249250985 11 -43.185532492509807 12 -43.185532492509857 13 -43.185532492509878
		 14 -43.18553249250985 15 -43.185532492509857 16 -43.185532492509864 17 -43.185532492470742
		 18 -43.185532492509857 19 -43.185532492509822 20 -43.185532492509864 21 -43.185532492509843
		 22 -43.185532492509857 23 -43.185532492509836 24 -43.18553249250985 25 -43.185532492509864
		 26 -43.185532492509878 27 -43.185532492509829 28 -43.185532492509857 29 -43.185532492509836
		 30 -43.185532492509864 31 -43.185532492509878 32 -43.185532492509857 33 -43.185532492509836
		 34 -43.185532492509893 35 -43.185532492509864 36 -43.185532492509878 37 -43.185532492509864
		 38 -43.185532492509864 39 -43.185532492509836 40 -43.185532492509878;
createNode animCurveTL -n "R_lowerlip1_jnt_translateZ";
	rename -uid "1B24E7D7-4883-7C54-3C9B-28BD9420B79F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -22.048100000000026 1 -22.048099999891605
		 2 -22.0481 3 -22.048100000000009 4 -22.0481 5 -22.048100000000019 6 -22.048100000000019
		 7 -22.048100000000026 8 -22.048099999999998 9 -22.048100000000012 10 -22.048099999999991
		 11 -22.0481 12 -22.048099999999984 13 -22.048100000000009 14 -22.048100000000009
		 15 -22.048099999999998 16 -22.0481 17 -22.048100000061329 18 -22.048099999999998
		 19 -22.048099999999994 20 -22.048099999999998 21 -22.048100000000012 22 -22.048099999999998
		 23 -22.048100000000009 24 -22.048099999999987 25 -22.048100000000005 26 -22.048099999999991
		 27 -22.048100000000009 28 -22.048100000000005 29 -22.048100000000012 30 -22.048100000000012
		 31 -22.048100000000005 32 -22.048099999999998 33 -22.048100000000009 34 -22.048099999999998
		 35 -22.048100000000009 36 -22.048100000000009 37 -22.048100000000005 38 -22.048100000000009
		 39 -22.048100000000005 40 -22.048100000000026;
createNode animCurveTA -n "R_lowerlip1_jnt_rotateX";
	rename -uid "4DD568A8-4C7E-1F2E-C06C-D8AA4722011B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_lowerlip1_jnt_rotateY";
	rename -uid "AA4AFF53-4BA0-C927-0115-A5A22A0A81D8";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_lowerlip1_jnt_rotateZ";
	rename -uid "04E20EBB-4BCE-534E-75E4-F88DE944A985";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.4033418597069752e-14 1 0 2 -3.9477856048777889e-14
		 3 0 4 -3.9477856048777889e-14 5 -1.4033418597069755e-14 6 -1.4033418597069752e-14
		 7 -1.4033418597069752e-14 8 0 9 0 10 -1.4033418597069752e-14 11 -1.4033418597069752e-14
		 12 -3.9477856048777889e-14 13 -1.4033418597069755e-14 14 -1.4033418597069752e-14
		 15 -1.4033418597069752e-14 16 -1.4033418597069752e-14 17 -1.4033418597069752e-14
		 18 -3.9477856048777889e-14 19 -3.9477856048777889e-14 20 -1.4033418597069755e-14
		 21 -1.4033418597069752e-14 22 -1.4033418597069752e-14 23 -3.9477856048777889e-14
		 24 -1.4033418597069755e-14 25 -3.9477856048777889e-14 26 -1.4033418597069755e-14
		 27 -3.9477856048777889e-14 28 0 29 -1.4033418597069752e-14 30 -3.9477856048777889e-14
		 31 -3.9477856048777889e-14 32 -1.4033418597069755e-14 33 -1.4033418597069752e-14
		 34 -1.4033418597069752e-14 35 -1.4033418597069752e-14 36 -3.9477856048777889e-14
		 37 -1.4033418597069755e-14 38 -1.4033418597069752e-14 39 -1.4033418597069752e-14
		 40 -1.4033418597069752e-14;
createNode animCurveTU -n "R_lowerlip1_jnt_scaleX";
	rename -uid "028F9DDE-4224-1FAB-748E-97A82998F71B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_lowerlip1_jnt_scaleY";
	rename -uid "7BF1FC79-4DAE-8874-9579-32AB6CC64CCF";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_lowerlip1_jnt_scaleZ";
	rename -uid "B7888585-4E1F-BE0B-E49E-189185A338BC";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_lowerlip2_jnt_visibility";
	rename -uid "EAF4ADC0-4E83-066B-0D25-23981DD183E9";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "R_lowerlip2_jnt_translateX";
	rename -uid "EC8E3FF0-4D1F-C478-B190-E596CE106512";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -18.229920825387822 1 -18.229920825357567
		 2 -18.22992082538785 3 -18.229920825387836 4 -18.229920825387865 5 -18.229920825387865
		 6 -18.22992082538785 7 -18.229920825387794 8 -18.22992082538785 9 -18.229920825387808
		 10 -18.22992082538785 11 -18.22992082538785 12 -18.229920825387836 13 -18.229920825387836
		 14 -18.229920825387794 15 -18.229920825387836 16 -18.229920825387794 17 -18.229920825413757
		 18 -18.229920825387794 19 -18.229920825387865 20 -18.229920825387865 21 -18.229920825387808
		 22 -18.229920825387808 23 -18.229920825387836 24 -18.229920825387836 25 -18.229920825387822
		 26 -18.229920825387808 27 -18.229920825387836 28 -18.229920825387836 29 -18.22992082538785
		 30 -18.22992082538785 31 -18.229920825387822 32 -18.229920825387822 33 -18.229920825387865
		 34 -18.229920825387836 35 -18.229920825387808 36 -18.229920825387808 37 -18.229920825387808
		 38 -18.229920825387836 39 -18.229920825387879 40 -18.229920825387808;
createNode animCurveTL -n "R_lowerlip2_jnt_translateY";
	rename -uid "C0BBFA0C-4E58-BDF0-79CA-D381CF1B168A";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -27.625677023041192 1 -27.625677023153891
		 2 -27.625677023041192 3 -27.625677023041227 4 -27.625677023041177 5 -27.625677023041185
		 6 -27.625677023041256 7 -27.625677023041206 8 -27.62567702304122 9 -27.625677023041199
		 10 -27.625677023041199 11 -27.625677023041192 12 -27.625677023041234 13 -27.62567702304122
		 14 -27.625677023041192 15 -27.625677023041213 16 -27.62567702304122 17 -27.625677022972859
		 18 -27.625677023041206 19 -27.625677023041192 20 -27.625677023041241 21 -27.625677023041192
		 22 -27.625677023041199 23 -27.625677023041213 24 -27.625677023041199 25 -27.62567702304122
		 26 -27.625677023041234 27 -27.625677023041192 28 -27.625677023041206 29 -27.625677023041199
		 30 -27.625677023041213 31 -27.625677023041227 32 -27.625677023041206 33 -27.625677023041185
		 34 -27.62567702304122 35 -27.62567702304122 36 -27.62567702304122 37 -27.625677023041206
		 38 -27.625677023041206 39 -27.625677023041206 40 -27.625677023041234;
createNode animCurveTL -n "R_lowerlip2_jnt_translateZ";
	rename -uid "6944C44C-4FF0-8E9E-620C-999B68074504";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -38.537800000000004 1 -38.537799999933547
		 2 -38.53779999999999 3 -38.537800000000004 4 -38.537799999999983 5 -38.537800000000004
		 6 -38.537800000000004 7 -38.537800000000011 8 -38.537799999999976 9 -38.53779999999999
		 10 -38.53779999999999 11 -38.53779999999999 12 -38.537799999999983 13 -38.53779999999999
		 14 -38.53779999999999 15 -38.53779999999999 16 -38.53779999999999 17 -38.537800000036725
		 18 -38.537799999999983 19 -38.537799999999983 20 -38.537799999999976 21 -38.5378
		 22 -38.537799999999983 23 -38.537800000000004 24 -38.537799999999976 25 -38.53779999999999
		 26 -38.537799999999969 27 -38.537800000000018 28 -38.537799999999969 29 -38.5378
		 30 -38.5378 31 -38.537799999999983 32 -38.537799999999976 33 -38.537800000000004
		 34 -38.537799999999983 35 -38.5378 36 -38.5378 37 -38.53779999999999 38 -38.5378
		 39 -38.5378 40 -38.537800000000004;
createNode animCurveTA -n "R_lowerlip2_jnt_rotateX";
	rename -uid "FE4E4878-4894-3C26-2667-83A4743B872A";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_lowerlip2_jnt_rotateY";
	rename -uid "189FFD1D-48FB-B041-BAAE-2FA23A2BDA55";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_lowerlip2_jnt_rotateZ";
	rename -uid "72B57B86-4518-EDB1-610A-40A3181CD276";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.4033418597069752e-14 1 0 2 -3.9477856048777889e-14
		 3 0 4 -3.9477856048777889e-14 5 -1.4033418597069755e-14 6 -1.4033418597069752e-14
		 7 -1.4033418597069752e-14 8 0 9 0 10 -1.4033418597069752e-14 11 -1.4033418597069752e-14
		 12 -3.9477856048777889e-14 13 -1.4033418597069755e-14 14 -1.4033418597069752e-14
		 15 -1.4033418597069752e-14 16 -1.4033418597069752e-14 17 -1.4033418597069752e-14
		 18 -3.9477856048777889e-14 19 -3.9477856048777889e-14 20 -1.4033418597069755e-14
		 21 -1.4033418597069752e-14 22 -1.4033418597069752e-14 23 -3.9477856048777889e-14
		 24 -1.4033418597069755e-14 25 -3.9477856048777889e-14 26 -1.4033418597069755e-14
		 27 -3.9477856048777889e-14 28 0 29 -1.4033418597069752e-14 30 -3.9477856048777889e-14
		 31 -3.9477856048777889e-14 32 -1.4033418597069755e-14 33 -1.4033418597069752e-14
		 34 -1.4033418597069752e-14 35 -1.4033418597069752e-14 36 -3.9477856048777889e-14
		 37 -1.4033418597069755e-14 38 -1.4033418597069752e-14 39 -1.4033418597069752e-14
		 40 -1.4033418597069752e-14;
createNode animCurveTU -n "R_lowerlip2_jnt_scaleX";
	rename -uid "02DF0F27-4AB1-2178-9D9F-65BC9B507B4F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_lowerlip2_jnt_scaleY";
	rename -uid "00E07E1C-4E49-8124-BF9E-A295C6E1524C";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_lowerlip2_jnt_scaleZ";
	rename -uid "BC37526D-4507-0396-34B6-6BA112E83897";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_scapula_jnt_visibility";
	rename -uid "7F59FC41-4BF6-3C0D-620D-40A769918198";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "R_scapula_jnt_translateX";
	rename -uid "3287BAD3-4E0E-5FE1-C728-A3B99DCC5212";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 2.2670174998968093 1 2.2670174998962196
		 2 2.2670174998968093 3 2.2670174998961983 4 2.267017499896788 5 2.267017499896177
		 6 2.2670174998968093 7 2.2670174998968093 8 2.2670174998967951 9 2.2670174998968164
		 10 2.2670174998968093 11 2.2670174998968093 12 2.2670174998968236 13 2.2670174998961841
		 14 2.2670174998961912 15 2.2670174998967951 16 2.2670174998967951 17 2.2670174998961627
		 18 2.2670174998968164 19 2.2670174998961983 20 2.2670174998968236 21 2.2670174998968022
		 22 2.2670174998967951 23 2.2670174998961627 24 2.2670174998968236 25 2.267017499896177
		 26 2.2670174998961699 27 2.2670174998961699 28 2.2670174998968022 29 2.2670174998961912
		 30 2.2670174998968378 31 2.2670174998968022 32 2.2670174998961841 33 2.2670174998968164
		 34 2.2670174998968093 35 2.267017499896177 36 2.2670174998968022 37 2.2670174998961699
		 38 2.2670174998968093 39 2.2670174998961699 40 2.2670174998968093;
createNode animCurveTL -n "R_scapula_jnt_translateY";
	rename -uid "231C4395-442A-1F25-72CC-B19F499DECB8";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 39.83788500491157 1 39.837885004911513
		 2 39.837885004911541 3 39.837885004911541 4 39.837885004911612 5 39.837885004911627
		 6 39.837885004911541 7 39.837885004911584 8 39.837885004911641 9 39.837885004911556
		 10 39.837885004911556 11 39.83788500491157 12 39.83788500491157 13 39.837885004911584
		 14 39.837885004911627 15 39.837885004911627 16 39.837885004911541 17 39.837885004911584
		 18 39.837885004911556 19 39.837885004911541 20 39.837885004911527 21 39.837885004911541
		 22 39.837885004911584 23 39.837885004911584 24 39.837885004911598 25 39.837885004911598
		 26 39.837885004911584 27 39.837885004911612 28 39.837885004911584 29 39.837885004911584
		 30 39.837885004911499 31 39.837885004911598 32 39.837885004911584 33 39.837885004911556
		 34 39.83788500491157 35 39.837885004911598 36 39.837885004911541 37 39.837885004911598
		 38 39.83788500491157 39 39.837885004911627 40 39.83788500491157;
createNode animCurveTL -n "R_scapula_jnt_translateZ";
	rename -uid "0164F9E2-41FC-85A3-61A1-678008A49D34";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -36.956399999999995 1 -36.956399999999995
		 2 -36.956399999999995 3 -36.9564 4 -36.956399999999988 5 -36.9564 6 -36.9564 7 -36.956399999999995
		 8 -36.9564 9 -36.956399999999995 10 -36.956399999999995 11 -36.956399999999995 12 -36.956399999999981
		 13 -36.956399999999995 14 -36.956399999999995 15 -36.9564 16 -36.956399999999995
		 17 -36.956399999999988 18 -36.9564 19 -36.956399999999995 20 -36.956399999999988
		 21 -36.956400000000009 22 -36.956399999999995 23 -36.956399999999988 24 -36.956400000000009
		 25 -36.956399999999988 26 -36.9564 27 -36.9564 28 -36.956399999999988 29 -36.9564
		 30 -36.956399999999988 31 -36.956399999999995 32 -36.956399999999995 33 -36.956399999999995
		 34 -36.956399999999988 35 -36.956399999999995 36 -36.956399999999995 37 -36.9564
		 38 -36.9564 39 -36.956399999999988 40 -36.956399999999995;
createNode animCurveTA -n "R_scapula_jnt_rotateX";
	rename -uid "F2EC56D1-40FA-9591-980E-6EA7CA3F12B9";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.1330726052713777e-14 1 4.3991046937992475e-13
		 2 -1.1330726052713779e-14 3 4.3991046937992475e-13 4 -1.1330726052713779e-14 5 4.4229588539102239e-13
		 6 -1.1330726052713779e-14 7 -1.3517357396219947e-14 8 -1.3517357396219947e-14 9 -1.3517357396219947e-14
		 10 -1.3517357396219947e-14 11 -1.3517357396219947e-14 12 -7.7526020360673203e-15
		 13 4.4030803871510767e-13 14 4.4229588539102239e-13 15 -1.1330726052713779e-14 16 -1.3517357396219947e-14
		 17 4.4229588539102239e-13 18 -1.1330726052713779e-14 19 4.3991046937992475e-13 20 -1.3517357396219947e-14
		 21 -1.3517357396219947e-14 22 -1.1330726052713777e-14 23 4.4030803871510767e-13 24 -1.3517357396219947e-14
		 25 4.4229588539102239e-13 26 4.4229588539102239e-13 27 4.4030803871510767e-13 28 -1.2722218725854067e-14
		 29 4.3991046937992475e-13 30 -1.1330726052713779e-14 31 -1.3517357396219947e-14 32 4.4030803871510767e-13
		 33 -1.1330726052713779e-14 34 -1.3517357396219947e-14 35 4.4030803871510767e-13 36 -1.3517357396219947e-14
		 37 4.4229588539102239e-13 38 -1.1330726052713779e-14 39 4.4030803871510767e-13 40 -1.1330726052713779e-14;
createNode animCurveTA -n "R_scapula_jnt_rotateY";
	rename -uid "6E5F1763-4909-7481-1E36-D993FE24C39E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.5902773407317584e-14 1 -3.4230719759250833e-13
		 2 1.5902773407317584e-14 3 -3.4230719759250833e-13 4 1.5902773407317584e-14 5 -3.44295044268423e-13
		 6 1.5902773407317584e-14 7 1.7890620083232281e-14 8 1.7890620083232281e-14 9 1.7890620083232281e-14
		 10 1.7890620083232281e-14 11 1.7890620083232281e-14 12 6.7586786981099743e-15 13 -3.5145129230171597e-13
		 14 -3.44295044268423e-13 15 1.5902773407317584e-14 16 1.7890620083232281e-14 17 -3.44295044268423e-13
		 18 1.5902773407317584e-14 19 -3.4230719759250833e-13 20 1.7890620083232281e-14 21 1.7890620083232281e-14
		 22 1.5902773407317584e-14 23 -3.5145129230171597e-13 24 1.7890620083232281e-14 25 -3.44295044268423e-13
		 26 -3.44295044268423e-13 27 -3.5145129230171597e-13 28 2.6239576122074014e-14 29 -3.4230719759250833e-13
		 30 1.5902773407317584e-14 31 1.7890620083232281e-14 32 -3.5145129230171597e-13 33 1.5902773407317584e-14
		 34 1.7890620083232281e-14 35 -3.5145129230171597e-13 36 1.7890620083232281e-14 37 -3.44295044268423e-13
		 38 1.5902773407317584e-14 39 -3.5145129230171597e-13 40 1.5902773407317584e-14;
createNode animCurveTA -n "R_scapula_jnt_rotateZ";
	rename -uid "E1F4FE80-48B5-04D3-E0C1-9C98335144B3";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 6.361109362927032e-15 1 -6.9336092055904805e-13
		 2 6.3611093629270335e-15 3 -6.9336092055904805e-13 4 6.3611093629270335e-15 5 -6.8699981119612101e-13
		 6 6.3611093629270335e-15 7 6.3611093629270312e-15 8 6.3611093629270312e-15 9 6.3611093629270312e-15
		 10 6.3611093629270312e-15 11 6.3611093629270312e-15 12 1.2722218725854067e-14 13 -6.8699981119612101e-13
		 14 -6.8699981119612101e-13 15 6.3611093629270335e-15 16 6.3611093629270312e-15 17 -6.8699981119612101e-13
		 18 6.3611093629270335e-15 19 -6.9336092055904805e-13 20 6.3611093629270335e-15 21 6.3611093629270312e-15
		 22 6.361109362927032e-15 23 -6.8699981119612101e-13 24 6.3611093629270335e-15 25 -6.8699981119612101e-13
		 26 -6.8699981119612101e-13 27 -6.8699981119612101e-13 28 6.3611093629270335e-15 29 -6.9336092055904805e-13
		 30 6.3611093629270335e-15 31 6.3611093629270312e-15 32 -6.8699981119612101e-13 33 6.3611093629270335e-15
		 34 6.3611093629270312e-15 35 -6.8699981119612101e-13 36 6.3611093629270335e-15 37 -6.8699981119612101e-13
		 38 6.3611093629270335e-15 39 -6.8699981119612101e-13 40 6.3611093629270335e-15;
createNode animCurveTU -n "R_scapula_jnt_scaleX";
	rename -uid "6E49BA3D-49BA-BB4A-3507-F897AB300F05";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_scapula_jnt_scaleY";
	rename -uid "E9B432D9-4537-BC03-1C97-ACA6A901F513";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_scapula_jnt_scaleZ";
	rename -uid "C40EDD50-4A2B-C089-9B07-2FA82C55D1B2";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_upperlip1_jnt_visibility";
	rename -uid "B2B0A1BF-40B3-DAF0-DE96-0B885325186B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "R_upperlip1_jnt_translateX";
	rename -uid "50B25694-4C1F-8C7E-6FD6-4B8AEC87BD23";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 41.141382753382757 1 41.141383084540522
		 2 41.141382753382743 3 41.141382753382743 4 41.141382753382729 5 41.141382753382743
		 6 41.141382753382764 7 41.141382753382771 8 41.141382753382729 9 41.141382753382715
		 10 41.141382753382729 11 41.14138275338275 12 41.141382753382736 13 41.14138275338275
		 14 41.141382753382729 15 41.141382753382764 16 41.14138275338275 17 41.141382995885436
		 18 41.141382753382715 19 41.14138275338275 20 41.141382753382729 21 41.141382753382743
		 22 41.141382753382715 23 41.141382753382729 24 41.141382753382743 25 41.141382753382743
		 26 41.141382753382736 27 41.14138275338275 28 41.141382753382743 29 41.141382753382764
		 30 41.141382753382743 31 41.141382753382722 32 41.14138275338275 33 41.141382753382771
		 34 41.141382753382729 35 41.141382753382715 36 41.141382753382736 37 41.14138275338275
		 38 41.141382753382757 39 41.141382753382764 40 41.14138275338275;
createNode animCurveTL -n "R_upperlip1_jnt_translateY";
	rename -uid "01155B6D-42AC-9E4F-5D86-4296CB05C685";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 27.391061093427624 1 27.391060596014455
		 2 27.391061093427609 3 27.39106109342768 4 27.391061093427609 5 27.391061093427666
		 6 27.391061093427638 7 27.391061093427638 8 27.391061093427638 9 27.391061093427609
		 10 27.391061093427595 11 27.391061093427652 12 27.391061093427609 13 27.391061093427652
		 14 27.391061093427638 15 27.391061093427624 16 27.391061093427624 17 27.391060729197307
		 18 27.391061093427666 19 27.391061093427624 20 27.391061093427666 21 27.391061093427638
		 22 27.391061093427624 23 27.391061093427652 24 27.391061093427624 25 27.391061093427624
		 26 27.391061093427624 27 27.391061093427624 28 27.391061093427638 29 27.391061093427652
		 30 27.391061093427638 31 27.391061093427652 32 27.391061093427624 33 27.391061093427638
		 34 27.391061093427638 35 27.391061093427652 36 27.391061093427638 37 27.391061093427666
		 38 27.391061093427624 39 27.391061093427638 40 27.391061093427652;
createNode animCurveTL -n "R_upperlip1_jnt_translateZ";
	rename -uid "7C8C52EF-401B-239F-14E7-CE9139E7F0EF";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 17.555300000000027 1 17.55530000002193
		 2 17.555300000000006 3 17.5553 4 17.555300000000006 5 17.555300000000006 6 17.555300000000003
		 7 17.555300000000017 8 17.555299999999988 9 17.555299999999995 10 17.555299999999995
		 11 17.555300000000003 12 17.555299999999988 13 17.555299999999995 14 17.555300000000003
		 15 17.555300000000003 16 17.555300000000003 17 17.555299999986264 18 17.555299999999992
		 19 17.555299999999995 20 17.555300000000003 21 17.555300000000003 22 17.555300000000003
		 23 17.555300000000006 24 17.555299999999995 25 17.555300000000003 26 17.5553 27 17.555300000000003
		 28 17.555300000000003 29 17.555300000000006 30 17.555300000000003 31 17.5553 32 17.555300000000006
		 33 17.555300000000003 34 17.555299999999992 35 17.555300000000003 36 17.555300000000003
		 37 17.555300000000003 38 17.555300000000013 39 17.555300000000006 40 17.555300000000024;
createNode animCurveTA -n "R_upperlip1_jnt_rotateX";
	rename -uid "91912B01-4CEE-CB30-F7FF-0C9A0AD1EE66";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_upperlip1_jnt_rotateY";
	rename -uid "C83ADF7B-45E4-7A12-C65F-6EB2C9B5C06F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_upperlip1_jnt_rotateZ";
	rename -uid "0BCD1900-4C03-12E0-86BC-64AC56FE2A54";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 3.9477856048777889e-14 1 -6.9265827241881776e-07
		 2 6.4922293544110776e-14 3 3.9477856048777889e-14 4 6.4922293500486023e-14 5 6.4922293500486023e-14
		 6 3.9477856048777889e-14 7 3.9477856048777889e-14 8 6.4922293500486023e-14 9 3.9477856048777889e-14
		 10 6.4922293500486023e-14 11 3.9477856048777889e-14 12 6.4922293500486023e-14 13 3.9477856048777889e-14
		 14 3.9477856048777889e-14 15 3.9477856048777889e-14 16 3.9477856048777889e-14 17 -5.0728936258482326e-07
		 18 6.4922293544110776e-14 19 6.4922293500486023e-14 20 6.4922293500486023e-14 21 6.4922293500486023e-14
		 22 6.4922293500486023e-14 23 6.4922293500486023e-14 24 3.9477856048777889e-14 25 6.4922293500486023e-14
		 26 1.4033418597069755e-14 27 6.4922293500486023e-14 28 3.9477856048777889e-14 29 6.4922293500486023e-14
		 30 6.4922293500486023e-14 31 6.4922293500486023e-14 32 3.9477856048777889e-14 33 6.4922293500486023e-14
		 34 3.9477856048777889e-14 35 3.9477856048777889e-14 36 3.9477856048777889e-14 37 3.9477856048777889e-14
		 38 1.4033418597069755e-14 39 6.4922293500486023e-14 40 3.9477856048777889e-14;
createNode animCurveTU -n "R_upperlip1_jnt_scaleX";
	rename -uid "6F6EE551-4E7C-AB98-54C8-2DB08F4FB900";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_upperlip1_jnt_scaleY";
	rename -uid "55E8934D-48A4-8322-A6C1-37AABBA7B0E9";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_upperlip1_jnt_scaleZ";
	rename -uid "A17E28BF-4E6A-466A-038B-50BED76B5077";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_upperlip2_jnt_visibility";
	rename -uid "D7C788F3-4077-0AB5-187A-9684EF1318E7";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "R_upperlip2_jnt_translateX";
	rename -uid "A3DE984D-4803-D2E2-35A5-28BFD2894AEE";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 38.007550208145197 1 38.007550385686159
		 2 38.007550208145204 3 38.007550208145204 4 38.007550208145176 5 38.007550208145204
		 6 38.007550208145226 7 38.007550208145254 8 38.007550208145204 9 38.00755020814519
		 10 38.007550208145204 11 38.007550208145204 12 38.007550208145233 13 38.007550208145226
		 14 38.007550208145204 15 38.007550208145219 16 38.007550208145226 17 38.007550338116928
		 18 38.00755020814519 19 38.007550208145204 20 38.007550208145204 21 38.007550208145197
		 22 38.007550208145176 23 38.007550208145204 24 38.007550208145197 25 38.007550208145226
		 26 38.007550208145197 27 38.007550208145219 28 38.007550208145211 29 38.007550208145219
		 30 38.00755020814519 31 38.007550208145204 32 38.007550208145219 33 38.007550208145233
		 34 38.007550208145204 35 38.007550208145204 36 38.007550208145219 37 38.00755020814524
		 38 38.007550208145204 39 38.007550208145219 40 38.007550208145197;
createNode animCurveTL -n "R_upperlip2_jnt_translateY";
	rename -uid "2B42E890-4A35-19DE-6F71-D0A4325AB635";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 14.682556945873188 1 14.682556486306339
		 2 14.682556945873174 3 14.682556945873216 4 14.682556945873117 5 14.682556945873202
		 6 14.682556945873174 7 14.682556945873174 8 14.682556945873174 9 14.682556945873174
		 10 14.682556945873159 11 14.682556945873159 12 14.682556945873145 13 14.682556945873188
		 14 14.682556945873202 15 14.682556945873159 16 14.682556945873159 17 14.682556609413894
		 18 14.682556945873202 19 14.682556945873159 20 14.682556945873174 21 14.682556945873174
		 22 14.682556945873188 23 14.682556945873188 24 14.682556945873159 25 14.682556945873159
		 26 14.682556945873159 27 14.682556945873188 28 14.682556945873174 29 14.682556945873188
		 30 14.682556945873202 31 14.682556945873159 32 14.682556945873131 33 14.682556945873202
		 34 14.682556945873174 35 14.682556945873188 36 14.682556945873174 37 14.682556945873202
		 38 14.682556945873159 39 14.682556945873202 40 14.682556945873188;
createNode animCurveTL -n "R_upperlip2_jnt_translateZ";
	rename -uid "F8CE154C-49EE-7053-BE5B-22A9776CDAAD";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 31.825500000000012 1 31.825499999991155
		 2 31.825500000000009 3 31.825499999999991 4 31.825499999999987 5 31.825500000000005
		 6 31.825499999999995 7 31.825500000000005 8 31.825499999999977 9 31.825499999999977
		 10 31.825499999999991 11 31.825499999999995 12 31.825499999999963 13 31.825499999999991
		 14 31.825499999999991 15 31.825499999999998 16 31.825499999999995 17 31.825500000005526
		 18 31.825499999999973 19 31.825499999999977 20 31.825499999999991 21 31.825499999999987
		 22 31.825499999999984 23 31.825499999999998 24 31.82549999999998 25 31.825499999999987
		 26 31.825499999999987 27 31.825499999999998 28 31.825499999999991 29 31.825499999999998
		 30 31.825499999999991 31 31.825499999999984 32 31.825499999999995 33 31.825499999999995
		 34 31.825499999999987 35 31.825499999999991 36 31.825499999999995 37 31.825499999999995
		 38 31.8255 39 31.825499999999998 40 31.825500000000009;
createNode animCurveTA -n "R_upperlip2_jnt_rotateX";
	rename -uid "5647BFF6-4046-DBDA-4353-F188D6F0B4F2";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_upperlip2_jnt_rotateY";
	rename -uid "F317B4EF-462B-0F96-1AC2-9BA97C7BB0B6";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "R_upperlip2_jnt_rotateZ";
	rename -uid "81261743-409B-365E-CB7F-BEB3C213760B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 3.9477856048777889e-14 1 -6.9265827241881776e-07
		 2 6.4922293544110776e-14 3 3.9477856048777889e-14 4 6.4922293500486023e-14 5 6.4922293500486023e-14
		 6 3.9477856048777889e-14 7 3.9477856048777889e-14 8 6.4922293500486023e-14 9 3.9477856048777889e-14
		 10 6.4922293500486023e-14 11 3.9477856048777889e-14 12 6.4922293500486023e-14 13 3.9477856048777889e-14
		 14 3.9477856048777889e-14 15 3.9477856048777889e-14 16 3.9477856048777889e-14 17 -5.0728936258482326e-07
		 18 6.4922293544110776e-14 19 6.4922293500486023e-14 20 6.4922293500486023e-14 21 6.4922293500486023e-14
		 22 6.4922293500486023e-14 23 6.4922293500486023e-14 24 3.9477856048777889e-14 25 6.4922293500486023e-14
		 26 1.4033418597069755e-14 27 6.4922293500486023e-14 28 3.9477856048777889e-14 29 6.4922293500486023e-14
		 30 6.4922293500486023e-14 31 6.4922293500486023e-14 32 3.9477856048777889e-14 33 6.4922293500486023e-14
		 34 3.9477856048777889e-14 35 3.9477856048777889e-14 36 3.9477856048777889e-14 37 3.9477856048777889e-14
		 38 1.4033418597069755e-14 39 6.4922293500486023e-14 40 3.9477856048777889e-14;
createNode animCurveTU -n "R_upperlip2_jnt_scaleX";
	rename -uid "C2A6053C-43E9-D645-F9E6-8588567FB0AB";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_upperlip2_jnt_scaleY";
	rename -uid "AB51C356-4E51-E0E0-552C-47B0790A2397";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "R_upperlip2_jnt_scaleZ";
	rename -uid "EE0E02A4-47E3-1C00-9C02-7E9712D17A96";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "browmid_jnt_visibility";
	rename -uid "A1A754EB-41C5-5B9E-F6DE-52941D99398E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "browmid_jnt_translateX";
	rename -uid "8C1DACF1-44AB-7E94-0D98-78B4A64D5675";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 20.661338633019263 1 20.661339144069611
		 2 20.661338633019241 3 20.661338633019241 4 20.661338633019238 5 20.661338633019238
		 6 20.661338633019277 7 20.661338633019263 8 20.661338633019252 9 20.661338633019216
		 10 20.661338633019259 11 20.66133863301927 12 20.661338633019266 13 20.661338633019255
		 14 20.661338633019259 15 20.661338633019277 16 20.661338633019255 17 20.661339007302541
		 18 20.661338633019209 19 20.661338633019255 20 20.661338633019241 21 20.661338633019255
		 22 20.661338633019199 23 20.661338633019231 24 20.661338633019263 25 20.661338633019255
		 26 20.661338633019234 27 20.661338633019263 28 20.661338633019277 29 20.661338633019255
		 30 20.661338633019248 31 20.661338633019234 32 20.66133863301927 33 20.661338633019266
		 34 20.661338633019266 35 20.661338633019234 36 20.661338633019248 37 20.661338633019255
		 38 20.661338633019255 39 20.661338633019263 40 20.661338633019263;
createNode animCurveTL -n "browmid_jnt_translateY";
	rename -uid "CAFA2DFA-4A7A-CB11-D0D2-1BB3A77DB0BD";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 42.273409913134657 1 42.273409663356233
		 2 42.273409913134671 3 42.273409913134742 4 42.273409913134643 5 42.273409913134728
		 6 42.273409913134699 7 42.273409913134699 8 42.273409913134671 9 42.273409913134699
		 10 42.273409913134685 11 42.273409913134742 12 42.273409913134671 13 42.273409913134714
		 14 42.273409913134671 15 42.273409913134714 16 42.273409913134685 17 42.273409730201877
		 18 42.273409913134699 19 42.273409913134657 20 42.273409913134699 21 42.273409913134671
		 22 42.273409913134714 23 42.273409913134685 24 42.273409913134657 25 42.273409913134685
		 26 42.273409913134685 27 42.273409913134685 28 42.273409913134728 29 42.273409913134714
		 30 42.273409913134671 31 42.273409913134685 32 42.273409913134657 33 42.273409913134699
		 34 42.273409913134699 35 42.273409913134714 36 42.273409913134699 37 42.273409913134728
		 38 42.273409913134685 39 42.273409913134671 40 42.273409913134714;
createNode animCurveTL -n "browmid_jnt_translateZ";
	rename -uid "2DCDEB86-46F9-1895-2D1F-F6A9C1502D91";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -2.4178890027850471e-14 1 8.9071625075654026e-11
		 2 -3.5360603334311236e-14 3 -3.5749181392930041e-14 4 -3.4861002973229915e-14 5 -3.5527136788005009e-14
		 6 -3.730349362740526e-14 7 -2.8421709430404007e-14 8 -3.907985046680551e-14 9 -4.0856207306205761e-14
		 10 -3.730349362740526e-14 11 -3.907985046680551e-14 12 -4.9737991503207013e-14 13 -3.730349362740526e-14
		 14 -3.730349362740526e-14 15 -3.730349362740526e-14 16 -3.6415315207705135e-14 17 -5.5883575544868336e-11
		 18 -3.907985046680551e-14 19 -3.730349362740526e-14 20 -3.3750779948604759e-14 21 -3.4638958368304884e-14
		 22 -3.730349362740526e-14 23 -3.6415315207705135e-14 24 -3.6415315207705135e-14 25 -3.730349362740526e-14
		 26 -3.730349362740526e-14 27 -3.6415315207705135e-14 28 -3.4194869158454821e-14 29 -3.5527136788005009e-14
		 30 -3.4194869158454821e-14 31 -3.5971225997855072e-14 32 -3.5971225997855072e-14
		 33 -3.5554892363620638e-14 34 -3.6193270602780103e-14 35 -3.5305092183079978e-14
		 36 -3.5083047578154947e-14 37 -3.5083047578154947e-14 38 -3.5527136788005009e-14
		 39 -3.5527136788005009e-14 40 -2.4190866069633737e-14;
createNode animCurveTA -n "browmid_jnt_rotateX";
	rename -uid "33234A90-457D-590B-A6C6-F184987558B7";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "browmid_jnt_rotateY";
	rename -uid "BF4AC451-4EE2-B6B6-6D20-C6AFCC591EDC";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "browmid_jnt_rotateZ";
	rename -uid "663E073E-4C4C-E2D1-BEC1-8EA6A61BF423";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "browmid_jnt_scaleX";
	rename -uid "C653197D-4D4C-AF8D-D532-E691232D500A";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "browmid_jnt_scaleY";
	rename -uid "DA727388-4C2A-1422-BF2D-2EA7B2793495";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "browmid_jnt_scaleZ";
	rename -uid "AEC79900-4AE9-C9C9-CE75-5A9E823C864C";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "fluffbase_jnt_visibility";
	rename -uid "1149CE5E-4587-6D38-10CC-00A0E353129D";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "fluffbase_jnt_translateX";
	rename -uid "CB5CA405-4E71-1EE6-9353-15967F17C9F9";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -14.183754193480384 1 -14.183753670304153
		 2 -14.183754193480405 3 -14.183754193480398 4 -14.1837541934804 5 -14.183754193480414
		 6 -14.18375419348037 7 -14.183754193480363 8 -14.183754193480386 9 -14.183754193480407
		 10 -14.183754193480386 11 -14.183754193480382 12 -14.183754193480386 13 -14.183754193480391
		 14 -14.183754193480372 15 -14.183754193480384 16 -14.183754193480398 17 -14.183753810324237
		 18 -14.183754193480414 19 -14.183754193480375 20 -14.183754193480411 21 -14.183754193480389
		 22 -14.183754193480427 23 -14.183754193480393 24 -14.183754193480373 25 -14.183754193480375
		 26 -14.183754193480389 27 -14.183754193480384 28 -14.183754193480375 29 -14.183754193480384
		 30 -14.183754193480416 31 -14.183754193480402 32 -14.183754193480361 33 -14.183754193480386
		 34 -14.183754193480393 35 -14.183754193480405 36 -14.183754193480388 37 -14.183754193480391
		 38 -14.183754193480384 39 -14.183754193480391 40 -14.183754193480384;
createNode animCurveTL -n "fluffbase_jnt_translateY";
	rename -uid "BAFF7E08-45A0-0BEF-30A2-53866F53AA5B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 43.275964925394362 1 43.275965096851792
		 2 43.275964925394319 3 43.275964925394391 4 43.275964925394348 5 43.275964925394376
		 6 43.275964925394376 7 43.275964925394348 8 43.275964925394319 9 43.275964925394376
		 10 43.275964925394305 11 43.275964925394391 12 43.275964925394348 13 43.275964925394391
		 14 43.275964925394348 15 43.275964925394391 16 43.275964925394362 17 43.275965050983189
		 18 43.275964925394376 19 43.275964925394334 20 43.275964925394348 21 43.275964925394348
		 22 43.275964925394334 23 43.275964925394334 24 43.275964925394391 25 43.275964925394362
		 26 43.275964925394391 27 43.275964925394334 28 43.275964925394376 29 43.275964925394362
		 30 43.275964925394348 31 43.275964925394362 32 43.275964925394334 33 43.275964925394348
		 34 43.275964925394376 35 43.275964925394362 36 43.275964925394348 37 43.275964925394405
		 38 43.275964925394391 39 43.275964925394348 40 43.275964925394391;
createNode animCurveTL -n "fluffbase_jnt_translateZ";
	rename -uid "74B32866-4675-C0E7-7D7D-A198EE9B20AF";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 4.49162675699794 1 4.4916267571347568
		 2 4.4916267569979444 3 4.4916267569979453 4 4.4916267569979444 5 4.4916267569979436
		 6 4.4916267569979436 7 4.4916267569979382 8 4.4916267569979418 9 4.4916267569979382
		 10 4.4916267569979453 11 4.4916267569979382 12 4.4916267569979365 13 4.4916267569979418
		 14 4.4916267569979373 15 4.4916267569979409 16 4.4916267569979427 17 4.4916267569121926
		 18 4.4916267569979373 19 4.4916267569979436 20 4.49162675699794 21 4.4916267569979409
		 22 4.4916267569979418 23 4.4916267569979462 24 4.4916267569979427 25 4.4916267569979436
		 26 4.4916267569979418 27 4.4916267569979436 28 4.4916267569979436 29 4.4916267569979436
		 30 4.4916267569979453 31 4.49162675699794 32 4.4916267569979436 33 4.4916267569979436
		 34 4.4916267569979409 35 4.4916267569979418 36 4.4916267569979418 37 4.4916267569979427
		 38 4.4916267569979436 39 4.4916267569979453 40 4.49162675699794;
createNode animCurveTA -n "fluffbase_jnt_rotateX";
	rename -uid "7470FB56-4DAA-578A-9B7B-E9B50DB11A98";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -2.0525091295681913e-14 1 -2.2665889615283707e-07
		 2 2.0972911449767405e-14 3 1.4678550138730088e-14 4 2.0972911469405876e-14 5 1.1394535531420985e-14
		 6 2.0549970194222124e-14 7 1.1319898835800322e-14 8 2.0549970194222124e-14 9 7.6875796489281339e-15
		 10 2.4381320569416085e-14 11 1.3683394197121285e-15 12 3.6099281781859944e-14 13 2.3908621497151892e-14
		 14 2.0500212397141683e-14 15 2.3336406830726825e-14 16 2.0549970194222124e-14 17 -1.6609377166032766e-07
		 18 2.6918968225559052e-14 19 2.0549970194222124e-14 20 2.0972911469405876e-14 21 1.4678550138730088e-14
		 22 3.0227861726367907e-14 23 1.1394535531420985e-14 24 8.0358842284912175e-15 25 1.1394535531420985e-14
		 26 2.0549970194222124e-14 27 1.4678550138730088e-14 28 1.3782909791282154e-14 29 1.1394535531420985e-14
		 30 1.7664017963556551e-14 31 1.7664017963556551e-14 32 1.4404882254787667e-14 33 1.7664017963556551e-14
		 34 2.0500212397141683e-14 35 1.1444293328501426e-14 36 2.7192636104460997e-14 37 2.0549970194222124e-14
		 38 8.210036518272764e-15 39 1.4678550138730088e-14 40 -1.1444293328501428e-14;
createNode animCurveTA -n "fluffbase_jnt_rotateY";
	rename -uid "49C3ADB0-49BA-25BD-C5AD-4B972C69E5F1";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -2.8519940937865518 1 -2.8519937053150257
		 2 -2.8519940937865602 3 -2.8519940937865567 4 -2.8519940937865602 5 -2.851994093786554
		 6 -2.8519940937865464 7 -2.8519940937865536 8 -2.8519940937865464 9 -2.8519940937865504
		 10 -2.8519940937865642 11 -2.8519940937865393 12 -2.8519940937865473 13 -2.85199409378655
		 14 -2.8519940937865451 15 -2.8519940937865442 16 -2.8519940937865464 17 -2.8519938091243939
		 18 -2.8519940937865593 19 -2.8519940937865464 20 -2.8519940937865602 21 -2.8519940937865567
		 22 -2.8519940937865615 23 -2.851994093786554 24 -2.8519940937865438 25 -2.851994093786554
		 26 -2.8519940937865464 27 -2.8519940937865567 28 -2.8519940937865456 29 -2.851994093786554
		 30 -2.851994093786558 31 -2.851994093786558 32 -2.8519940937865438 33 -2.851994093786558
		 34 -2.8519940937865451 35 -2.8519940937865549 36 -2.8519940937865509 37 -2.8519940937865464
		 38 -2.8519940937865469 39 -2.8519940937865567 40 -2.851994093786542;
createNode animCurveTA -n "fluffbase_jnt_rotateZ";
	rename -uid "66F72073-4011-CA51-AD44-B7AC4CA37B1E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -4.7840403871732411 1 -4.7840409027906192
		 2 -4.7840403871732313 3 -4.7840403871732411 4 -4.7840403871732313 5 -4.7840403871732402
		 6 -4.7840403871732384 7 -4.7840403871732393 8 -4.7840403871732384 9 -4.784040387173226
		 10 -4.784040387173234 11 -4.7840403871732464 12 -4.7840403871732216 13 -4.7840403871732393
		 14 -4.7840403871732384 15 -4.7840403871732251 16 -4.7840403871732384 17 -4.7840407646436152
		 18 -4.7840403871732162 19 -4.7840403871732384 20 -4.7840403871732313 21 -4.7840403871732411
		 22 -4.7840403871732171 23 -4.7840403871732402 24 -4.7840403871732509 25 -4.7840403871732402
		 26 -4.7840403871732384 27 -4.7840403871732411 28 -4.7840403871732242 29 -4.7840403871732402
		 30 -4.7840403871732313 31 -4.7840403871732313 32 -4.7840403871732518 33 -4.7840403871732313
		 34 -4.7840403871732384 35 -4.7840403871732411 36 -4.7840403871732411 37 -4.7840403871732384
		 38 -4.7840403871732535 39 -4.7840403871732411 40 -4.7840403871732393;
createNode animCurveTU -n "fluffbase_jnt_scaleX";
	rename -uid "1B88C8C5-437C-B33A-BF5E-8BBF87AE68EB";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 0.99999999999999989 2 1 3 1.0000000000000004
		 4 1 5 1.0000000000000002 6 1 7 1.0000000000000004 8 0.99999999999999978 9 0.99999999999999989
		 10 0.99999999999999989 11 0.99999999999999989 12 0.99999999999999978 13 1 14 0.99999999999999967
		 15 1.0000000000000002 16 1.0000000000000004 17 1.0000000000000002 18 0.99999999999999978
		 19 0.99999999999999967 20 1 21 1 22 0.99999999999999989 23 1.0000000000000002 24 0.99999999999999978
		 25 1.0000000000000002 26 1 27 1.0000000000000002 28 1 29 1.0000000000000002 30 1.0000000000000002
		 31 1 32 1.0000000000000002 33 1 34 1 35 1 36 1 37 1.0000000000000002 38 1.0000000000000004
		 39 1 40 1.0000000000000002;
createNode animCurveTU -n "fluffbase_jnt_scaleY";
	rename -uid "24C4C64E-43BE-AE69-B5FB-5F9213174695";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.99999999999999978 1 0.99999999999999967
		 2 0.99999999999999978 3 1 4 0.99999999999999978 5 0.99999999999999978 6 0.99999999999999967
		 7 1 8 0.99999999999999967 9 1 10 0.99999999999999967 11 0.99999999999999978 12 0.99999999999999967
		 13 0.99999999999999978 14 0.99999999999999967 15 1 16 1 17 0.99999999999999989 18 0.99999999999999967
		 19 0.99999999999999956 20 0.99999999999999989 21 0.99999999999999978 22 0.99999999999999967
		 23 0.99999999999999978 24 0.99999999999999978 25 0.99999999999999978 26 0.99999999999999956
		 27 0.99999999999999978 28 1 29 0.99999999999999978 30 0.99999999999999978 31 0.99999999999999967
		 32 0.99999999999999978 33 0.99999999999999967 34 0.99999999999999989 35 0.99999999999999989
		 36 0.99999999999999978 37 1 38 0.99999999999999989 39 0.99999999999999978 40 0.99999999999999978;
createNode animCurveTU -n "fluffbase_jnt_scaleZ";
	rename -uid "88FFB86C-4B3D-6682-1CE7-40BD2CF1A1CB";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.99999999999999989 1 0.99999999999999933
		 2 0.99999999999999967 3 0.99999999999999967 4 0.99999999999999933 5 1.0000000000000002
		 6 1 7 0.99999999999999978 8 0.99999999999999944 9 0.99999999999999922 10 0.99999999999999933
		 11 0.99999999999999967 12 0.99999999999999956 13 1 14 0.99999999999999944 15 0.99999999999999978
		 16 0.99999999999999956 17 0.99999999999999933 18 0.99999999999999933 19 0.99999999999999911
		 20 0.99999999999999978 21 0.99999999999999978 22 0.99999999999999944 23 0.99999999999999989
		 24 0.99999999999999944 25 0.99999999999999978 26 0.99999999999999911 27 0.99999999999999978
		 28 0.99999999999999967 29 0.99999999999999967 30 1 31 0.99999999999999956 32 0.99999999999999967
		 33 0.99999999999999989 34 0.99999999999999911 35 0.99999999999999933 36 0.99999999999999944
		 37 0.99999999999999967 38 0.99999999999999978 39 0.99999999999999989 40 0.99999999999999989;
createNode animCurveTU -n "flufftip_jnt_visibility";
	rename -uid "92AC2CE6-4339-DAA6-DCE3-C69D41D8E5D9";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "flufftip_jnt_translateX";
	rename -uid "77E93482-48DB-0E03-F64F-F599E56D78F2";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 28.116980793683069 1 28.116980793683183
		 2 28.116980793683098 3 28.116980793683069 4 28.116980793683069 5 28.116980793683126
		 6 28.116980793683098 7 28.116980793683126 8 28.116980793683126 9 28.116980793683126
		 10 28.116980793683155 11 28.116980793683098 12 28.116980793683126 13 28.116980793683126
		 14 28.116980793683155 15 28.116980793683098 16 28.116980793683155 17 28.116980793683126
		 18 28.116980793683098 19 28.116980793683126 20 28.116980793683126 21 28.116980793683069
		 22 28.116980793683126 23 28.116980793683098 24 28.116980793683069 25 28.116980793683126
		 26 28.116980793683126 27 28.116980793683069 28 28.116980793683126 29 28.116980793683126
		 30 28.116980793683183 31 28.116980793683126 32 28.116980793683098 33 28.116980793683126
		 34 28.116980793683155 35 28.116980793683098 36 28.116980793683126 37 28.116980793683126
		 38 28.116980793683098 39 28.116980793683098 40 28.116980793683069;
createNode animCurveTL -n "flufftip_jnt_translateY";
	rename -uid "49F4F87E-436E-33DA-AB9C-BEACBD1C81EA";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.4210854715202004e-14 1 2.1316282072803006e-14
		 2 0 3 4.2632564145606011e-14 4 2.1316282072803006e-14 5 2.1316282072803006e-14 6 -7.1054273576010019e-15
		 7 7.1054273576010019e-15 8 0 9 -7.1054273576010019e-15 10 1.4210854715202004e-14
		 11 -2.1316282072803006e-14 12 -2.1316282072803006e-14 13 -2.8421709430404007e-14
		 14 -7.1054273576010019e-15 15 0 16 -1.4210854715202004e-14 17 -7.1054273576010019e-15
		 18 1.0658141036401503e-14 19 0 20 -7.1054273576010019e-15 21 2.4868995751603507e-14
		 22 4.2632564145606011e-14 23 3.5527136788005009e-15 24 3.5527136788005009e-15 25 2.8421709430404007e-14
		 26 3.5527136788005009e-15 27 1.7763568394002505e-14 28 0 29 2.1316282072803006e-14
		 30 7.1054273576010019e-15 31 2.1316282072803006e-14 32 1.4210854715202004e-14 33 7.1054273576010019e-15
		 34 -1.4210854715202004e-14 35 2.1316282072803006e-14 36 2.1316282072803006e-14 37 7.1054273576010019e-15
		 38 0 39 0 40 7.1054273576010019e-15;
createNode animCurveTL -n "flufftip_jnt_translateZ";
	rename -uid "B489EE1D-4780-45D4-5223-DF9ECDF4BE6A";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 5.6843418860808015e-14 1 3.5527136788005009e-14
		 2 4.2632564145606011e-14 3 5.6843418860808015e-14 4 5.6843418860808015e-14 5 5.6843418860808015e-14
		 6 5.6843418860808015e-14 7 6.3948846218409017e-14 8 2.1316282072803006e-14 9 3.5527136788005009e-14
		 10 4.2632564145606011e-14 11 2.8421709430404007e-14 12 2.8421709430404007e-14 13 3.5527136788005009e-14
		 14 4.2632564145606011e-14 15 3.5527136788005009e-14 16 4.2632564145606011e-14 17 0
		 18 6.3948846218409017e-14 19 4.9737991503207013e-14 20 2.1316282072803006e-14 21 7.1054273576010019e-14
		 22 4.2632564145606011e-14 23 3.5527136788005009e-14 24 4.9737991503207013e-14 25 3.5527136788005009e-14
		 26 4.2632564145606011e-14 27 4.2632564145606011e-14 28 2.1316282072803006e-14 29 2.8421709430404007e-14
		 30 4.9737991503207013e-14 31 5.6843418860808015e-14 32 4.9737991503207013e-14 33 4.2632564145606011e-14
		 34 2.8421709430404007e-14 35 5.6843418860808015e-14 36 5.6843418860808015e-14 37 3.5527136788005009e-14
		 38 5.6843418860808015e-14 39 4.9737991503207013e-14 40 3.5527136788005009e-14;
createNode animCurveTA -n "flufftip_jnt_rotateX";
	rename -uid "739B2955-441D-1BBA-5CEC-CA943FD99215";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "flufftip_jnt_rotateY";
	rename -uid "E973E99F-4F09-8E35-8E01-C2AC95A29743";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "flufftip_jnt_rotateZ";
	rename -uid "84493747-4AA4-B88E-C6D1-EDA8BFB0834D";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "flufftip_jnt_scaleX";
	rename -uid "47614551-4A88-9818-0B0C-53B34D65E175";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1.0000000000000002 2 1.0000000000000002
		 3 1.0000000000000002 4 1.0000000000000002 5 1.0000000000000002 6 1 7 1 8 1.0000000000000002
		 9 1 10 1.0000000000000004 11 1.0000000000000002 12 0.99999999999999989 13 1 14 1.0000000000000004
		 15 1 16 1.0000000000000002 17 1.0000000000000002 18 1 19 1.0000000000000004 20 0.99999999999999989
		 21 1.0000000000000002 22 1.0000000000000002 23 1.0000000000000002 24 1 25 1.0000000000000002
		 26 1.0000000000000004 27 1.0000000000000004 28 1.0000000000000002 29 1.0000000000000004
		 30 1.0000000000000004 31 1.0000000000000004 32 1.0000000000000002 33 1 34 1.0000000000000007
		 35 1.0000000000000002 36 1.0000000000000002 37 0.99999999999999978 38 1.0000000000000002
		 39 1.0000000000000002 40 1.0000000000000002;
createNode animCurveTU -n "flufftip_jnt_scaleY";
	rename -uid "0F437024-40DD-460A-C20B-6F9341046F38";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.99999999999999944 1 0.99999999999999989
		 2 0.99999999999999967 3 0.99999999999999956 4 0.99999999999999956 5 0.99999999999999967
		 6 0.99999999999999956 7 0.99999999999999944 8 1 9 0.99999999999999978 10 0.99999999999999967
		 11 0.99999999999999944 12 0.99999999999999978 13 0.99999999999999989 14 1 15 0.99999999999999944
		 16 0.99999999999999944 17 0.99999999999999933 18 0.99999999999999944 19 0.99999999999999989
		 20 0.99999999999999956 21 0.99999999999999978 22 0.99999999999999967 23 0.99999999999999933
		 24 0.99999999999999967 25 0.99999999999999956 26 0.99999999999999944 27 0.99999999999999944
		 28 0.99999999999999989 29 0.99999999999999956 30 0.99999999999999956 31 0.99999999999999967
		 32 0.99999999999999956 33 0.99999999999999956 34 1 35 1 36 0.99999999999999989 37 0.99999999999999956
		 38 0.99999999999999956 39 0.99999999999999967 40 0.99999999999999956;
createNode animCurveTU -n "flufftip_jnt_scaleZ";
	rename -uid "7B7478D0-4434-2A46-862C-66A4FBB980A0";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.0000000000000002 1 1.0000000000000002
		 2 1.0000000000000004 3 0.99999999999999989 4 1 5 1.0000000000000002 6 1 7 0.99999999999999989
		 8 1.0000000000000004 9 1 10 1.0000000000000002 11 1 12 0.99999999999999978 13 1 14 1.0000000000000004
		 15 1 16 0.99999999999999989 17 0.99999999999999967 18 0.99999999999999956 19 1 20 1.0000000000000002
		 21 0.99999999999999989 22 0.99999999999999956 23 1.0000000000000002 24 0.99999999999999967
		 25 1.0000000000000004 26 1 27 1 28 1 29 1 30 1.0000000000000004 31 0.99999999999999989
		 32 1.0000000000000002 33 1.0000000000000004 34 1 35 1.0000000000000002 36 1 37 0.99999999999999978
		 38 1 39 1.0000000000000004 40 1.0000000000000002;
createNode animCurveTU -n "head_jnt_visibility";
	rename -uid "3C58F84E-4A6E-3346-FD7C-D9A4B6E0CA26";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "head_jnt_translateX";
	rename -uid "3BC27257-4383-C4B2-C44A-EDA1856A3479";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -32.626709853925888 1 -32.626709853925725
		 2 -32.626709853925888 3 -32.62670985392576 4 -32.626709853925902 5 -32.626709853925774
		 6 -32.626709853925888 7 -32.626709853925888 8 -32.626709853925888 9 -32.626709853925895
		 10 -32.626709853925888 11 -32.626709853925888 12 -32.626709853925867 13 -32.62670985392576
		 14 -32.626709853925746 15 -32.626709853925895 16 -32.626709853925895 17 -32.626709853925774
		 18 -32.626709853925881 19 -32.626709853925746 20 -32.626709853925874 21 -32.626709853925902
		 22 -32.626709853925902 23 -32.626709853925789 24 -32.626709853925874 25 -32.626709853925774
		 26 -32.626709853925774 27 -32.626709853925774 28 -32.626709853925895 29 -32.62670985392576
		 30 -32.626709853925874 31 -32.626709853925895 32 -32.62670985392576 33 -32.62670985392586
		 34 -32.626709853925888 35 -32.626709853925774 36 -32.626709853925895 37 -32.626709853925774
		 38 -32.626709853925888 39 -32.626709853925774 40 -32.626709853925888;
createNode animCurveTL -n "head_jnt_translateY";
	rename -uid "227A80E4-47CC-E199-ED59-4A9ED384A4F0";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -7.4171821103610682 1 -7.4171821103616224
		 2 -7.4171821103610398 3 -7.4171821103616082 4 -7.4171821103610398 5 -7.4171821103615514
		 6 -7.4171821103610824 7 -7.4171821103610398 8 -7.4171821103609972 9 -7.417182110361054
		 10 -7.417182110361054 11 -7.417182110361054 12 -7.417182110361054 13 -7.417182110361594
		 14 -7.4171821103615514 15 -7.4171821103610256 16 -7.417182110361054 17 -7.4171821103615514
		 18 -7.4171821103610682 19 -7.417182110361594 20 -7.4171821103610966 21 -7.417182110361054
		 22 -7.417182110361054 23 -7.4171821103615798 24 -7.4171821103610256 25 -7.4171821103615798
		 26 -7.4171821103615798 27 -7.4171821103615656 28 -7.417182110361054 29 -7.417182110361594
		 30 -7.4171821103611251 31 -7.4171821103610682 32 -7.417182110361594 33 -7.4171821103610824
		 34 -7.4171821103610398 35 -7.417182110361594 36 -7.4171821103610398 37 -7.4171821103615514
		 38 -7.417182110361054 39 -7.4171821103615514 40 -7.4171821103610682;
createNode animCurveTL -n "head_jnt_translateZ";
	rename -uid "07DD6BAF-4032-274E-05D1-009E5939D260";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -2.2425047360110228e-31 1 0 2 0 3 -1.1102230246251565e-16
		 4 7.7715611723760958e-16 5 -2.2204460492503131e-16 6 -8.8817841970012523e-16 7 1.3322676295501878e-15
		 8 6.6613381477509392e-16 9 -1.1102230246251565e-15 10 -2.2204460492503131e-16 11 -4.4408920985006262e-16
		 12 6.6613381477509392e-16 13 -3.3306690738754696e-16 14 5.5511151231257827e-17 15 -2.7755575615628914e-17
		 16 1.1102230246251565e-16 17 -1.3045120539345589e-15 18 8.3266726846886741e-16 19 -8.8817841970012523e-16
		 20 0 21 1.1102230246251565e-16 22 -6.6613381477509392e-16 23 6.6613381477509392e-16
		 24 2.6645352591003757e-15 25 8.8817841970012523e-16 26 -2.6645352591003757e-15 27 0
		 28 -1.7763568394002505e-15 29 -8.8817841970012523e-16 30 2.6645352591003757e-15 31 8.8817841970012523e-16
		 32 0 33 8.8817841970012523e-16 34 0 35 -9.9920072216264089e-16 36 4.4408920985006262e-16
		 37 2.2204460492503131e-16 38 1.1102230246251565e-16 39 -6.9388939039072284e-17 40 -2.2425047360110228e-31;
createNode animCurveTA -n "head_jnt_rotateX";
	rename -uid "3598EF46-4E25-A7D7-2355-25BDE516C2AB";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 2.9907536014786587e-15 1 -0.0061154389982311635
		 2 -0.025235217866100348 3 -0.05872403820330109 4 0.20760510090010445 5 1.275764623034046
		 6 3.1649668342667598 7 4.9489750337470424 8 5.8026341547644922 9 6.2622449984636939
		 10 6.4927926029577234 11 6.4855393710612148 12 6.5050417924011184 13 6.2921447303774602
		 14 5.5064124146223801 15 4.0817136979815194 16 2.1683470150536532 17 0.0052335442540630975
		 18 -1.9893655609892968 19 -3.0100963769801612 20 -3.0633490301563704 21 -2.8868270129282272
		 22 -2.6376660018096816 23 -2.4283045093847759 24 -1.9622446208524724 25 -1.0802003734003669
		 26 -0.099420644757191298 27 0.75967576936113268 28 1.3364104811295758 29 1.3708965072991675
		 30 1.0418662895665931 31 0.88165803618372629 32 0.8996164016810021 33 0.98272334606183287
		 34 1.037333671816828 35 1.037891320180246 36 1.0127763696959609 37 0.83018767825695849
		 38 0.48352549717822763 39 0.1477962200574445 40 2.9817700138720468e-15;
createNode animCurveTA -n "head_jnt_rotateY";
	rename -uid "88D30823-4041-8E15-6B93-B286EBC6CFF6";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -2.4775137252592348e-14 1 0.012979067345234139
		 2 0.053557805496859241 3 0.12463241408310642 4 -0.44059986998385292 5 -2.705375799381899
		 6 -6.6832573043884178 7 -10.375455023306914 8 -12.110512320603013 9 -13.034574219942922
		 10 -13.495242624347744 11 -13.480779481000738 12 -13.519663392075518 13 -13.094428196779532
		 14 -11.51110381935989 15 -8.5905212375045075 16 -4.5910296599473179 17 -0.011107383082892212
		 18 4.2136576141586906 19 6.3592804432486503 20 6.4707332665068913 21 6.1010843201065477
		 22 5.5783653848508106 23 5.1383359424988981 24 4.1564382348914926 25 2.2911993035432188
		 26 0.21100378690470586 27 -1.6118210355911311 28 -2.8337532651338817 29 -2.9067405810317748
		 30 -2.2099805854228749 31 -1.8704433938178977 32 -1.9085111940536106 33 -2.0846547334795282
		 34 -2.2003766334800554 35 -2.2015582159665601 36 -2.1483409388924217 37 -1.7613274701987958
		 38 -1.026085732472148 39 -0.31367099549600985 40 -2.4798387282035857e-14;
createNode animCurveTA -n "head_jnt_rotateZ";
	rename -uid "55228FF4-4C1A-B3B6-E877-919BD09E19CF";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -6.3611093629270335e-15 1 -6.9265835448776918e-07
		 2 -1.179443778873799e-05 3 -6.3869645733186114e-05 4 -0.00079823795123338355 5 -0.030126186521378304
		 6 -0.18484495499663597 7 -0.44960046514033197 8 -0.61606061054543304 9 -0.71611041637745421
		 10 -0.76901090253216808 11 -0.76731914497395548 12 -0.77187196841372552 13 -0.72286918657115462
		 14 -0.55542877906072774 15 -0.30669504403533809 16 -0.086930157796876198 17 -5.072894314637105e-07
		 18 -0.073191489481899991 19 -0.16725592165610451 20 -0.17320580203414163 21 -0.15387831629121737
		 22 -0.12852686356557239 23 -0.10897555935134984 24 -0.071212259059407146 25 -0.021601569258660129
		 26 -0.00018306899562833164 27 -0.010686302638569194 28 -0.03305654785666675 29 -0.034783412195558583
		 30 -0.020096185353479283 31 -0.014392599857690895 32 -0.014984716351778841 33 -0.017880157447885049
		 34 -0.01992177871543099 35 -0.019943195024069472 36 -0.018990059545511254 37 -0.012761612785076339
		 38 -0.0043297675935406055 39 -0.00040456321102381212 40 -6.3611093629270335e-15;
createNode animCurveTU -n "head_jnt_scaleX";
	rename -uid "24362415-4F7F-FCEF-3EF9-E98515742606";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "head_jnt_scaleY";
	rename -uid "9453E507-4420-31D7-F88A-19AAC86AE976";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "head_jnt_scaleZ";
	rename -uid "EB1BAEE7-42E9-B04B-0E4C-FBB97FBD1A73";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "jaw_jnt_visibility";
	rename -uid "057E254E-4E6D-3F5D-191D-2C8C7B684485";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "jaw_jnt_translateX";
	rename -uid "A4AC051D-4C45-4A36-6947-CDACC8AEF37A";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 11.272207582396716 1 11.272207582396721
		 2 11.27220758239673 3 11.272207582396723 4 11.272207582396714 5 11.272207582396721
		 6 11.272207582396737 7 11.272207582396751 8 11.272207582396728 9 11.272207582396721
		 10 11.272207582396735 11 11.272207582396753 12 11.272207582396785 13 11.272207582396765
		 14 11.272207582396771 15 11.272207582396758 16 11.272207582396751 17 11.272207582396749
		 18 11.272207582396756 19 11.27220758239676 20 11.272207582396753 21 11.27220758239676
		 22 11.272207582396751 23 11.272207582396756 24 11.272207582396762 25 11.272207582396753
		 26 11.272207582396724 27 11.272207582396765 28 11.272207582396732 29 11.27220758239673
		 30 11.272207582396719 31 11.272207582396733 32 11.272207582396732 33 11.272207582396749
		 34 11.272207582396735 35 11.272207582396737 36 11.272207582396755 37 11.272207582396765
		 38 11.272207582396744 39 11.27220758239673 40 11.272207582396737;
createNode animCurveTL -n "jaw_jnt_translateY";
	rename -uid "BF3D80D1-40BC-2E8B-DD48-21B6E840EE1B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.8474111129762605e-13 1 -1.3627182227082812e-07
		 2 -2.2737367544323206e-13 3 -1.7053025658242404e-13 4 -2.2737367544323206e-13 5 -1.9895196601282805e-13
		 6 -1.8474111129762605e-13 7 -1.9895196601282805e-13 8 -2.2737367544323206e-13 9 -1.9895196601282805e-13
		 10 -2.2737367544323206e-13 11 -1.9895196601282805e-13 12 -2.1316282072803006e-13
		 13 -1.8474111129762605e-13 14 -1.8474111129762605e-13 15 -1.9895196601282805e-13
		 16 -2.1316282072803006e-13 17 -9.9802861086573103e-08 18 -1.8474111129762605e-13
		 19 -2.1316282072803006e-13 20 -1.8474111129762605e-13 21 -2.1316282072803006e-13
		 22 -1.9895196601282805e-13 23 -1.9895196601282805e-13 24 -2.1316282072803006e-13
		 25 -1.9895196601282805e-13 26 -1.8474111129762605e-13 27 -2.1316282072803006e-13
		 28 -2.1316282072803006e-13 29 -1.8474111129762605e-13 30 -2.1316282072803006e-13
		 31 -1.9895196601282805e-13 32 -1.9895196601282805e-13 33 -1.9895196601282805e-13
		 34 -2.1316282072803006e-13 35 -1.9895196601282805e-13 36 -2.1316282072803006e-13
		 37 -1.8474111129762605e-13 38 -1.9895196601282805e-13 39 -2.1316282072803006e-13
		 40 -1.7053025658242404e-13;
createNode animCurveTL -n "jaw_jnt_translateZ";
	rename -uid "D89D71D3-43B5-C38B-579A-AA8B03A38C09";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -2.1768317625695436e-15 1 -1.4551956861730275e-11
		 2 -7.049916206369744e-15 3 -7.3274719625260332e-15 4 -6.7723604502134549e-15 5 -7.9936057773011271e-15
		 6 -8.8817841970012523e-15 7 -8.8817841970012523e-15 8 -5.3290705182007514e-15 9 -8.8817841970012523e-15
		 10 -1.7763568394002505e-15 11 -7.1054273576010019e-15 12 -1.4210854715202004e-14
		 13 -7.1054273576010019e-15 14 -8.8817841970012523e-15 15 -3.5527136788005009e-15
		 16 -6.2172489379008766e-15 17 9.1093244058981782e-12 18 -9.7699626167013776e-15 19 -7.1054273576010019e-15
		 20 -7.1054273576010019e-15 21 -8.8817841970012523e-15 22 -5.3290705182007514e-15
		 23 -6.2172489379008766e-15 24 -6.2172489379008766e-15 25 -8.8817841970012523e-15
		 26 -8.8817841970012523e-15 27 -7.9936057773011271e-15 28 -6.4392935428259079e-15
		 29 -7.7715611723760958e-15 30 -5.773159728050814e-15 31 -8.4376949871511897e-15 32 -7.5495165674510645e-15
		 33 -7.7576833845682813e-15 34 -7.7715611723760958e-15 35 -6.8833827526759706e-15
		 36 -7.1054273576010019e-15 37 -7.1054273576010019e-15 38 -6.8833827526759706e-15
		 39 -7.1609385088322597e-15 40 -2.1576695732036768e-15;
createNode animCurveTA -n "jaw_jnt_rotateX";
	rename -uid "76E04E65-4A4D-182D-C769-C1A370CA4C9B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 7.1828864967344544e-32 1 -1.5114758262792142e-12
		 2 1.414972623033317e-15 3 2.2974745208915791e-15 4 3.2558073526636973e-15 5 4.1312924196909477e-15
		 6 4.7684691692477786e-15 7 5.0147303358814898e-15 8 4.3642183555977562e-15 9 3.0645051571688305e-15
		 10 1.5207636586601647e-15 11 -2.454227274447434e-16 12 -1.8145504919059607e-15 13 -2.7715793454537099e-15
		 14 -2.6171732527534221e-15 15 -1.8447421839425569e-15 16 -1.5406416598344846e-15
		 17 -2.1855048486922447e-12 18 -1.8836978092910217e-15 19 -1.8913224095844325e-15
		 20 -1.8066585512558957e-15 21 -1.6423514080358481e-15 22 -1.4110220575410916e-15
		 23 -1.1253060421097003e-15 24 -7.9788015080893449e-16 25 -4.4147361816343053e-16
		 26 4.2940148214013469e-16 27 1.8670559748763701e-15 28 3.2078162222354299e-15 29 3.798023409218946e-15
		 30 3.6862658457167044e-15 31 3.3864032354873726e-15 32 2.951435254576258e-15 33 2.4346268878211422e-15
		 34 1.8897620051292452e-15 35 1.3711023415574685e-15 36 9.3317065308992065e-16 37 6.3051218316872258e-16
		 38 5.1755474775672732e-16 39 5.175547477567146e-16 40 8.8278125961003172e-32;
createNode animCurveTA -n "jaw_jnt_rotateY";
	rename -uid "ED45C314-4272-936C-528E-CA95DAE0C097";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 7.0618847882525444e-30 1 -5.0026380131395073e-11
		 2 2.540506346596751e-14 3 2.5340501337971624e-14 4 2.5235275225688126e-14 5 2.5106808243520156e-14
		 6 2.4993621166526089e-14 7 2.4945377866295744e-14 8 2.5067369135562501e-14 9 2.5259220205215632e-14
		 10 2.5398950276111547e-14 11 2.5443253819406459e-14 12 2.5379653341726546e-14 13 2.5293037483974557e-14
		 14 2.5309480464817123e-14 15 2.5377476697037292e-14 16 2.539775227278726e-14 17 3.1322281848719581e-11
		 18 2.5374614869928653e-14 19 2.5374047701872311e-14 20 2.5380216352763072e-14 21 2.5391378046226846e-14
		 22 2.5405283190451151e-14 23 2.5419541371658911e-14 24 2.5431924514256347e-14 25 2.5440607270235383e-14
		 26 2.5440813894233479e-14 27 2.5375844798165165e-14 28 2.5241420568546669e-14 29 2.5159380266948402e-14
		 30 2.517599732579805e-14 31 2.5218082210203219e-14 32 2.5272681440083328e-14 33 2.5327692140244165e-14
		 34 2.5374163962539878e-14 35 2.5407468894064665e-14 36 2.5427319751914464e-14 37 2.5436624218255922e-14
		 38 2.5439173223926963e-14 39 2.5439173223926963e-14 40 5.649800061504203e-30;
createNode animCurveTA -n "jaw_jnt_rotateZ";
	rename -uid "89BB61DB-4E06-205B-BEC3-94A9E5FDF537";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.1655100635675268 1 1.7305846027866452
		 2 3.1878796196799022 3 5.1805084470259413 4 7.3515816469701551 5 9.3442104743161885
		 6 10.801506183867803 7 11.366580030428594 8 9.8761804195103799 9 6.91744498002884
		 10 3.4264974234450065 11 -0.55265142143134105 12 -4.0894757886677535 13 -6.2534499123314893
		 14 -5.9037918184355842 15 -4.1576378902174733 16 -3.4713399213694038 17 -3.9913301320528851
		 18 -4.2455945601206437 19 -4.262811069556065 20 -4.071659553500302 21 -3.7008193978050214
		 22 -3.1789699883219256 23 -2.534790710902624 24 -1.7969609513988998 25 -0.99416009566238328
		 26 0.96697208027155246 27 4.2080182761865572 28 7.2426328199592973 29 8.5844700394665008
		 30 8.3300475437223565 31 7.6481952551280612 32 6.6610359716408496 33 5.4906924912178035
		 34 4.2592876118161831 35 3.0889441313931645 36 2.1017848479059205 37 1.4199325593116316
		 38 1.1655100635675268 39 1.1655100635675015 40 1.1655100635675268;
createNode animCurveTU -n "jaw_jnt_scaleX";
	rename -uid "7080DE6F-437B-97CD-8B66-7DB0CDD5295E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "jaw_jnt_scaleY";
	rename -uid "1F8F07E3-4E7B-822B-74F6-A1BB8B2FE989";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "jaw_jnt_scaleZ";
	rename -uid "4FFC2F53-4A9A-599A-33EE-DF9A49F2DEA2";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "lowerlip_visibility";
	rename -uid "448CEA42-431D-7EB0-E021-40B5E5D035B3";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "lowerlip_translateX";
	rename -uid "0D827CF7-451A-1382-A810-799286C52652";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -28.115270299837377 1 -28.115270299837348
		 2 -28.115270299837377 3 -28.115270299837377 4 -28.115270299837391 5 -28.115270299837391
		 6 -28.115270299837363 7 -28.115270299837306 8 -28.115270299837348 9 -28.11527029983732
		 10 -28.115270299837348 11 -28.115270299837377 12 -28.115270299837348 13 -28.115270299837363
		 14 -28.11527029983732 15 -28.115270299837363 16 -28.11527029983732 17 -28.115270299837363
		 18 -28.115270299837334 19 -28.115270299837391 20 -28.115270299837391 21 -28.115270299837334
		 22 -28.115270299837363 23 -28.115270299837363 24 -28.115270299837348 25 -28.115270299837348
		 26 -28.115270299837306 27 -28.115270299837363 28 -28.115270299837363 29 -28.115270299837391
		 30 -28.115270299837363 31 -28.115270299837334 32 -28.115270299837348 33 -28.115270299837377
		 34 -28.115270299837348 35 -28.115270299837334 36 -28.115270299837334 37 -28.11527029983732
		 38 -28.115270299837363 39 -28.11527029983742 40 -28.115270299837363;
createNode animCurveTL -n "lowerlip_translateY";
	rename -uid "35453DDC-49E6-8F65-253A-9E87CDDE706F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -46.876398688731562 1 -46.876398688731555
		 2 -46.876398688731534 3 -46.876398688731598 4 -46.876398688731527 5 -46.876398688731527
		 6 -46.876398688731598 7 -46.876398688731541 8 -46.876398688731555 9 -46.87639868873157
		 10 -46.876398688731541 11 -46.876398688731527 12 -46.876398688731555 13 -46.876398688731562
		 14 -46.876398688731534 15 -46.87639868873157 16 -46.87639868873157 17 -46.87639868873157
		 18 -46.876398688731562 19 -46.87639868873152 20 -46.876398688731577 21 -46.876398688731534
		 22 -46.876398688731548 23 -46.876398688731548 24 -46.876398688731541 25 -46.876398688731548
		 26 -46.87639868873157 27 -46.876398688731527 28 -46.876398688731548 29 -46.876398688731527
		 30 -46.876398688731548 31 -46.876398688731555 32 -46.876398688731555 33 -46.876398688731527
		 34 -46.876398688731584 35 -46.876398688731555 36 -46.87639868873157 37 -46.876398688731548
		 38 -46.876398688731555 39 -46.876398688731527 40 -46.876398688731591;
createNode animCurveTL -n "lowerlip_translateZ";
	rename -uid "27890782-4E0F-599C-9B69-61923414561F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.2540958412709111e-14 1 1.1495766838454102e-10
		 2 8.6042284408449632e-15 3 7.8825834748386114e-15 4 6.9388939039072284e-15 5 5.773159728050814e-15
		 6 3.5527136788005009e-15 7 -7.1054273576010019e-15 8 1.0658141036401503e-14 9 -1.7763568394002505e-15
		 10 2.4868995751603507e-14 11 3.5527136788005009e-15 12 1.9539925233402755e-14 13 8.8817841970012523e-15
		 14 1.7763568394002505e-15 15 1.4210854715202004e-14 16 9.7699626167013776e-15 17 -6.4239641384133961e-11
		 18 1.0658141036401503e-14 19 6.2172489379008766e-15 20 8.8817841970012523e-15 21 1.7763568394002505e-15
		 22 1.0658141036401503e-14 23 1.2434497875801753e-14 24 1.1546319456101628e-14 25 -1.7763568394002505e-15
		 26 7.9936057773011271e-15 27 7.9936057773011271e-15 28 7.1054273576010019e-15 29 5.773159728050814e-15
		 30 6.6613381477509392e-15 31 7.1054273576010019e-15 32 7.1054273576010019e-15 33 8.0074835651089415e-15
		 34 7.7715611723760958e-15 35 7.9936057773011271e-15 36 8.4376949871511897e-15 37 7.7715611723760958e-15
		 38 8.659739592076221e-15 39 9.1593399531575415e-15 40 -1.2515690979688238e-14;
createNode animCurveTA -n "lowerlip_rotateX";
	rename -uid "3B8DD0D4-4399-8C0B-D0A0-E7ABDCDF623E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "lowerlip_rotateY";
	rename -uid "443D9F91-42BF-8611-F0C7-82A718DE51A8";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "lowerlip_rotateZ";
	rename -uid "F41D416D-48FA-EE90-B9FD-32A1336F6A2F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "lowerlip_scaleX";
	rename -uid "D67DAA07-4C5C-116F-1B73-E1B98D21AA8B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "lowerlip_scaleY";
	rename -uid "810C2E0B-4336-3237-7F01-87821F7DAF14";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "lowerlip_scaleZ";
	rename -uid "FC723BDB-4425-84C8-7F1F-82B2DEA8000B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "pelvis_jnt_visibility";
	rename -uid "19C56740-4604-8D1C-9CF9-C08669B91500";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "pelvis_jnt_translateX";
	rename -uid "D8E5E280-4DFF-2519-826D-7EB5D4344ECE";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 29.203502960895356 1 29.203502960895356
		 2 29.203502960895356 3 29.203502960895356 4 29.203502960895356 5 29.203502960895356
		 6 29.203502960895356 7 29.203502960895356 8 29.203502960895356 9 29.203502960895356
		 10 29.203502960895356 11 29.203502960895356 12 29.203502960895356 13 29.203502960895356
		 14 29.203502960895356 15 29.203502960895356 16 29.203502960895356 17 29.203502960895356
		 18 29.203502960895356 19 29.203502960895356 20 29.203502960895356 21 29.203502960895356
		 22 29.203502960895356 23 29.203502960895356 24 29.203502960895356 25 29.203502960895356
		 26 29.203502960895356 27 29.203502960895356 28 29.203502960895356 29 29.203502960895356
		 30 29.203502960895356 31 29.203502960895356 32 29.203502960895356 33 29.203502960895356
		 34 29.203502960895356 35 29.203502960895356 36 29.203502960895356 37 29.203502960895356
		 38 29.203502960895356 39 29.203502960895356 40 29.203502960895356;
createNode animCurveTL -n "pelvis_jnt_translateY";
	rename -uid "6455109B-4306-85FC-3060-D184A461F640";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -2.6372165420930071e-15 1 -2.6372165420930071e-15
		 2 -2.6372165420930071e-15 3 -2.6372165420930071e-15 4 -2.6372165420930071e-15 5 -2.6372165420930071e-15
		 6 -2.6372165420930071e-15 7 -2.6372165420930071e-15 8 -2.6372165420930071e-15 9 -2.6372165420930071e-15
		 10 -2.6372165420930071e-15 11 -2.6372165420930071e-15 12 -2.6372165420930071e-15
		 13 -2.6372165420930071e-15 14 -2.6372165420930071e-15 15 -2.6372165420930071e-15
		 16 -2.6372165420930071e-15 17 -2.6372165420930071e-15 18 -2.6372165420930071e-15
		 19 -2.6372165420930071e-15 20 -2.6372165420930071e-15 21 -2.6372165420930071e-15
		 22 -2.6372165420930071e-15 23 -2.6372165420930071e-15 24 -2.6372165420930071e-15
		 25 -2.6372165420930071e-15 26 -2.6372165420930071e-15 27 -2.6372165420930071e-15
		 28 -2.6372165420930071e-15 29 -2.6372165420930071e-15 30 -2.6372165420930071e-15
		 31 -2.6372165420930071e-15 32 -2.6372165420930071e-15 33 -2.6372165420930071e-15
		 34 -2.6372165420930071e-15 35 -2.6372165420930071e-15 36 -2.6372165420930071e-15
		 37 -2.6372165420930071e-15 38 -2.6372165420930071e-15 39 -2.6372165420930071e-15
		 40 -2.6372165420930071e-15;
createNode animCurveTL -n "pelvis_jnt_translateZ";
	rename -uid "9A24E229-4677-E83D-8F73-F98164460A6E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.932088381262407e-14 1 -1.932088381262407e-14
		 2 -1.932088381262407e-14 3 -1.932088381262407e-14 4 -1.932088381262407e-14 5 -1.932088381262407e-14
		 6 -1.932088381262407e-14 7 -1.932088381262407e-14 8 -1.932088381262407e-14 9 -1.932088381262407e-14
		 10 -1.932088381262407e-14 11 -1.932088381262407e-14 12 -1.932088381262407e-14 13 -1.932088381262407e-14
		 14 -1.932088381262407e-14 15 -1.932088381262407e-14 16 -1.932088381262407e-14 17 -1.932088381262407e-14
		 18 -1.932088381262407e-14 19 -1.932088381262407e-14 20 -1.932088381262407e-14 21 -1.932088381262407e-14
		 22 -1.932088381262407e-14 23 -1.932088381262407e-14 24 -1.932088381262407e-14 25 -1.932088381262407e-14
		 26 -1.932088381262407e-14 27 -1.932088381262407e-14 28 -1.932088381262407e-14 29 -1.932088381262407e-14
		 30 -1.932088381262407e-14 31 -1.932088381262407e-14 32 -1.932088381262407e-14 33 -1.932088381262407e-14
		 34 -1.932088381262407e-14 35 -1.932088381262407e-14 36 -1.932088381262407e-14 37 -1.932088381262407e-14
		 38 -1.932088381262407e-14 39 -1.932088381262407e-14 40 -1.932088381262407e-14;
createNode animCurveTA -n "pelvis_jnt_rotateX";
	rename -uid "1D623875-4984-4045-FA0D-83BAEFEDFB2E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "pelvis_jnt_rotateY";
	rename -uid "B35250E6-4268-E83E-03C2-57A63C26D119";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "pelvis_jnt_rotateZ";
	rename -uid "FDD1C47A-4AD9-2310-0F29-308F937D46D0";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "pelvis_jnt_scaleX";
	rename -uid "78664175-48B0-3080-22C0-EAAC396D81B4";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "pelvis_jnt_scaleY";
	rename -uid "4F0142AF-488C-F114-E5D3-6DB98C4CF309";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "pelvis_jnt_scaleZ";
	rename -uid "AAEAA04A-4EB8-B216-AB19-BEB42C0D60ED";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "pelvis_splineik_jnt_visibility";
	rename -uid "820B69B6-4057-F655-037D-458B229ECDD2";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "pelvis_splineik_jnt_translateX";
	rename -uid "50054275-469F-34EC-228A-378BADFEE9F8";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -4.1095592369626982e-14 1 -4.1078251911130792e-14
		 2 -4.113376306236205e-14 3 -4.0967229608668276e-14 4 -4.1078251911130792e-14 5 -4.1078251911130792e-14
		 6 -4.0856207306205761e-14 7 -4.1078251911130792e-14 8 -4.1744385725905886e-14 9 -4.1078251911130792e-14
		 10 -4.1300296516055823e-14 11 -4.1300296516055823e-14 12 -4.1300296516055823e-14
		 13 -4.1078251911130792e-14 14 -4.113376306236205e-14 15 -4.1036618547707349e-14 16 -4.1328052091671452e-14
		 17 -4.0079051188968151e-14 18 -4.035660694512444e-14 19 -4.1439074394133968e-14 20 -3.8774539135033592e-14
		 21 -4.0634162701280729e-14 22 -4.0634162701280729e-14 23 -4.1744385725905886e-14
		 24 -4.1300296516055823e-14 25 -4.1300296516055823e-14 26 -3.6415315207705135e-14
		 27 -4.0856207306205761e-14 28 -4.0856207306205761e-14 29 -4.0856207306205761e-14
		 30 -4.0856207306205761e-14 31 -4.3520742565306136e-14 32 -4.2632564145606011e-14
		 33 -4.1744385725905886e-14 34 -4.1855408028368402e-14 35 -4.1189274213593308e-14
		 36 -4.0634162701280729e-14 37 -4.241051954068098e-14 38 -4.1189274213593308e-14 39 -4.1112946380650328e-14
		 40 -4.1095592369626982e-14;
createNode animCurveTL -n "pelvis_splineik_jnt_translateY";
	rename -uid "BBC7E49D-43E5-E014-A75D-87896F581835";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -0.76727200107440297 1 -0.76727200107437454
		 2 -0.76727200107441718 3 -0.76727200107440297 4 -0.76727200107438875 5 -0.76727200107441718
		 6 -0.76727200107437454 7 -0.76727200107440297 8 -0.76727200107441718 9 -0.76727200107437454
		 10 -0.76727200107438875 11 -0.76727200107440297 12 -0.76727200107438875 13 -0.76727200107438875
		 14 -0.76727200107440297 15 -0.76727200107443139 16 -0.76727200107440297 17 -0.76727200107441718
		 18 -0.76727200107438875 19 -0.76727200107437454 20 -0.76727200107436033 21 -0.76727200107438875
		 22 -0.76727200107443139 23 -0.76727200107440297 24 -0.76727200107438875 25 -0.76727200107436033
		 26 -0.76727200107441718 27 -0.76727200107440297 28 -0.76727200107440297 29 -0.76727200107438875
		 30 -0.76727200107437454 31 -0.76727200107438875 32 -0.76727200107440297 33 -0.76727200107438875
		 34 -0.76727200107440297 35 -0.76727200107438875 36 -0.76727200107437454 37 -0.76727200107440297
		 38 -0.76727200107438875 39 -0.76727200107443139 40 -0.76727200107440297;
createNode animCurveTL -n "pelvis_splineik_jnt_translateZ";
	rename -uid "0D76C709-4CEF-F8EC-CBC6-04B40175A6C9";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -78.020525999181601 1 -78.020525999181615
		 2 -78.020525999181615 3 -78.020525999181629 4 -78.020525999181601 5 -78.020525999181586
		 6 -78.020525999181601 7 -78.020525999181586 8 -78.020525999181601 9 -78.020525999181572
		 10 -78.020525999181601 11 -78.020525999181615 12 -78.020525999181615 13 -78.020525999181615
		 14 -78.020525999181586 15 -78.020525999181615 16 -78.020525999181586 17 -78.020525999181586
		 18 -78.020525999181615 19 -78.020525999181586 20 -78.020525999181629 21 -78.020525999181615
		 22 -78.020525999181586 23 -78.020525999181615 24 -78.020525999181615 25 -78.020525999181615
		 26 -78.020525999181601 27 -78.020525999181572 28 -78.020525999181586 29 -78.020525999181615
		 30 -78.020525999181601 31 -78.020525999181586 32 -78.020525999181615 33 -78.020525999181615
		 34 -78.020525999181601 35 -78.020525999181586 36 -78.020525999181615 37 -78.020525999181586
		 38 -78.020525999181615 39 -78.020525999181615 40 -78.020525999181601;
createNode animCurveTA -n "pelvis_splineik_jnt_rotateX";
	rename -uid "08B9B6FE-4981-9D1B-8449-9EA8CEFCA35B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "pelvis_splineik_jnt_rotateY";
	rename -uid "F90D3445-4810-BC7E-FCB6-2E8DB296DB58";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "pelvis_splineik_jnt_rotateZ";
	rename -uid "A6F2C852-4038-A926-5DFA-36BE0753DCC1";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "pelvis_splineik_jnt_scaleX";
	rename -uid "D746EB2A-41E1-B0A6-95E5-47BACEAC5300";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "pelvis_splineik_jnt_scaleY";
	rename -uid "1C174F3D-4666-9E37-E93C-5CB4D15ADFE7";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "pelvis_splineik_jnt_scaleZ";
	rename -uid "899DF73F-4549-6022-8671-B8BA4B6DC836";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "spine1_jnt_visibility";
	rename -uid "7CDF4EF7-4D9A-AF12-B734-1B81D5BEC657";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "spine1_jnt_translateX";
	rename -uid "9CA9A09F-4F4D-1979-D968-FBA26C66E0FF";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 31.195242546064911 1 31.195242546064911
		 2 31.195242546064911 3 31.195242546064911 4 31.195242546064911 5 31.195242546064911
		 6 31.195242546064911 7 31.195242546064911 8 31.195242546064911 9 31.195242546064911
		 10 31.195242546064911 11 31.195242546064911 12 31.195242546064911 13 31.195242546064911
		 14 31.195242546064911 15 31.195242546064911 16 31.195242546064911 17 31.195242546064911
		 18 31.195242546064911 19 31.195242546064911 20 31.195242546064911 21 31.195242546064911
		 22 31.195242546064911 23 31.195242546064911 24 31.195242546064911 25 31.195242546064911
		 26 31.195242546064911 27 31.195242546064911 28 31.195242546064911 29 31.195242546064911
		 30 31.195242546064911 31 31.195242546064911 32 31.195242546064911 33 31.195242546064911
		 34 31.195242546064911 35 31.195242546064911 36 31.195242546064911 37 31.195242546064911
		 38 31.195242546064911 39 31.195242546064911 40 31.195242546064911;
createNode animCurveTL -n "spine1_jnt_translateY";
	rename -uid "849E98A4-4A5A-A370-A933-6DA1D4DE9BBD";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 5.6843418860808015e-14 1 5.6843418860808015e-14
		 2 5.6843418860808015e-14 3 5.6843418860808015e-14 4 5.6843418860808015e-14 5 5.6843418860808015e-14
		 6 5.6843418860808015e-14 7 5.6843418860808015e-14 8 5.6843418860808015e-14 9 5.6843418860808015e-14
		 10 5.6843418860808015e-14 11 5.6843418860808015e-14 12 5.6843418860808015e-14 13 5.6843418860808015e-14
		 14 5.6843418860808015e-14 15 5.6843418860808015e-14 16 5.6843418860808015e-14 17 5.6843418860808015e-14
		 18 5.6843418860808015e-14 19 5.6843418860808015e-14 20 5.6843418860808015e-14 21 5.6843418860808015e-14
		 22 5.6843418860808015e-14 23 5.6843418860808015e-14 24 5.6843418860808015e-14 25 5.6843418860808015e-14
		 26 5.6843418860808015e-14 27 5.6843418860808015e-14 28 5.6843418860808015e-14 29 5.6843418860808015e-14
		 30 5.6843418860808015e-14 31 5.6843418860808015e-14 32 5.6843418860808015e-14 33 5.6843418860808015e-14
		 34 5.6843418860808015e-14 35 5.6843418860808015e-14 36 5.6843418860808015e-14 37 5.6843418860808015e-14
		 38 5.6843418860808015e-14 39 5.6843418860808015e-14 40 5.6843418860808015e-14;
createNode animCurveTL -n "spine1_jnt_translateZ";
	rename -uid "8C70BBCA-4519-9F45-6506-2BA43C44F746";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -2.1774709172168699e-14 1 -2.1774709172168699e-14
		 2 -2.1774709172168699e-14 3 -2.1774709172168699e-14 4 -2.1774709172168699e-14 5 -2.1774709172168699e-14
		 6 -2.1774709172168699e-14 7 -2.1774709172168699e-14 8 -2.1774709172168699e-14 9 -2.1774709172168699e-14
		 10 -2.1774709172168699e-14 11 -2.1774709172168699e-14 12 -2.1774709172168699e-14
		 13 -2.1774709172168699e-14 14 -2.1774709172168699e-14 15 -2.1774709172168699e-14
		 16 -2.1774709172168699e-14 17 -2.1774709172168699e-14 18 -2.1774709172168699e-14
		 19 -2.1774709172168699e-14 20 -2.1774709172168699e-14 21 -2.1774709172168699e-14
		 22 -2.1774709172168699e-14 23 -2.1774709172168699e-14 24 -2.1774709172168699e-14
		 25 -2.1774709172168699e-14 26 -2.1774709172168699e-14 27 -2.1774709172168699e-14
		 28 -2.1774709172168699e-14 29 -2.1774709172168699e-14 30 -2.1774709172168699e-14
		 31 -2.1774709172168699e-14 32 -2.1774709172168699e-14 33 -2.1774709172168699e-14
		 34 -2.1774709172168699e-14 35 -2.1774709172168699e-14 36 -2.1774709172168699e-14
		 37 -2.1774709172168699e-14 38 -2.1774709172168699e-14 39 -2.1774709172168699e-14
		 40 -2.1774709172168699e-14;
createNode animCurveTA -n "spine1_jnt_rotateX";
	rename -uid "78EB0D1C-4DD6-877B-84DA-67B4631E212E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -3.3341953627318203e-15 1 -3.3337521220139522e-15
		 2 -3.3217184405861084e-15 3 -3.3255677141923509e-15 4 -3.3200827120352639e-15 5 -3.3103215751517972e-15
		 6 -3.4043202655986772e-15 7 -3.3209978695297302e-15 8 -3.3043342341824362e-15 9 -3.3359631763702578e-15
		 10 -3.3057646082256434e-15 11 -3.3079080596237466e-15 12 -3.3390918789200605e-15
		 13 -3.3534770308040291e-15 14 -3.3232152758284772e-15 15 -3.337757722820562e-15 16 -3.3583847178238896e-15
		 17 -3.2963806406416157e-15 18 -3.3665817091958735e-15 19 -3.2665542515413496e-15
		 20 -3.2166213019439209e-15 21 -3.4430577678887842e-15 22 -3.3879482305211646e-15
		 23 -3.4278614876983186e-15 24 -3.1724707325880885e-15 25 -3.1818528587381062e-15
		 26 -2.9340373227611417e-15 27 -3.255607759397009e-15 28 -3.6091818582399074e-15 29 -3.0251691739562269e-15
		 30 -3.6090957136812772e-15 31 -3.2718735634649775e-15 32 -3.1317627279950234e-15
		 33 -3.3470630575000121e-15 34 -3.1615795992514673e-15 35 -3.3822406669183328e-15
		 36 -3.1766840385731433e-15 37 -3.3266683360958564e-15 38 -3.3484359504168925e-15
		 39 -3.3438679546355684e-15 40 -3.3341953627318203e-15;
createNode animCurveTA -n "spine1_jnt_rotateY";
	rename -uid "6CC97862-4AFE-ED2A-237C-4BB23D670E45";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -2.0380791137311591e-14 1 -2.0376376010655513e-14
		 2 -2.0140800795524307e-14 3 -2.0512445185829736e-14 4 -2.0167995461231761e-14 5 -2.0765918571932805e-14
		 6 -2.0945711992984395e-14 7 -2.0588420749054048e-14 8 -2.0865460599276861e-14 9 -1.9468335993167727e-14
		 10 -2.084168003744836e-14 11 -1.9934763947183188e-14 12 -2.0287600337002954e-14 13 -2.0048441219537933e-14
		 14 -2.0115915288688499e-14 15 -2.0418691275803183e-14 16 -1.9966848886976451e-14
		 17 -2.0017502029480027e-14 18 -2.1137490987464365e-14 19 -2.0622288161351164e-14
		 20 -1.960097347868716e-14 21 -2.2044245591142079e-14 22 -2.1217903867759667e-14 23 -1.7069208123560004e-14
		 24 -1.8701344406202052e-14 25 -2.0287923310177684e-14 26 -2.0922838110695383e-14
		 27 -1.9061717490979451e-14 28 -1.6668522710989493e-14 29 -1.9407733280352479e-14
		 30 -2.3640197472838771e-14 31 -1.8791291750805273e-14 32 -1.4150449939844196e-14
		 33 -1.9283795864822763e-14 34 -1.8011133694287762e-14 35 -2.1748434641125739e-14
		 36 -1.906693660719556e-14 37 -1.8751586246905787e-14 38 -2.0894621553567588e-14 39 -2.0480471197471283e-14
		 40 -2.0380791137311591e-14;
createNode animCurveTA -n "spine1_jnt_rotateZ";
	rename -uid "D484A5CF-4ED8-A72D-C556-B890D3BE789C";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -6.8842592075580127 1 -6.8842592075579594
		 2 -6.8842592075580136 3 -6.8842592075582267 4 -6.884259207558066 5 -6.8842592075580127
		 6 -6.8842592075580127 7 -6.8842592075582267 8 -6.8842592075580136 9 -6.8842592075580136
		 10 -6.8842592075582258 11 -6.8842592075581717 12 -6.8842592075582791 13 -6.8842592075581202
		 14 -6.8842592075581202 15 -6.8842592075580136 16 -6.8842592075580136 17 -6.8842592075582267
		 18 -6.8842592075580127 19 -6.8842592075580127 20 -6.8842592075581193 21 -6.8842592075582258
		 22 -6.884259207558066 23 -6.8842592075579603 24 -6.8842592075581202 25 -6.8842592075581717
		 26 -6.8842592075580136 27 -6.8842592075582258 28 -6.8842592075580136 29 -6.8842592075579603
		 30 -6.8842592075581193 31 -6.8842592075581202 32 -6.8842592075581734 33 -6.8842592075581202
		 34 -6.8842592075579585 35 -6.8842592075579088 36 -6.8842592075580136 37 -6.8842592075581202
		 38 -6.8842592075580127 39 -6.8842592075579603 40 -6.8842592075580127;
createNode animCurveTU -n "spine1_jnt_scaleX";
	rename -uid "E1E8E7D6-4672-D1E6-99D8-718D45E912E9";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "spine1_jnt_scaleY";
	rename -uid "AA666249-4D94-32FC-7E20-7F8993FEDB0F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "spine1_jnt_scaleZ";
	rename -uid "52FC7A3A-479D-6E25-9D30-11B8E6702121";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "spine2_jnt_visibility";
	rename -uid "BBCF7513-4F09-3BA0-8E4A-D7970759630D";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "spine2_jnt_translateX";
	rename -uid "B36C690A-4412-E533-5465-6FB9D344F941";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 35.59712974939206 1 35.59712974939206
		 2 35.59712974939206 3 35.59712974939206 4 35.59712974939206 5 35.59712974939206 6 35.59712974939206
		 7 35.59712974939206 8 35.59712974939206 9 35.59712974939206 10 35.59712974939206
		 11 35.59712974939206 12 35.59712974939206 13 35.59712974939206 14 35.59712974939206
		 15 35.59712974939206 16 35.59712974939206 17 35.59712974939206 18 35.59712974939206
		 19 35.59712974939206 20 35.59712974939206 21 35.59712974939206 22 35.59712974939206
		 23 35.59712974939206 24 35.59712974939206 25 35.59712974939206 26 35.59712974939206
		 27 35.59712974939206 28 35.59712974939206 29 35.59712974939206 30 35.59712974939206
		 31 35.59712974939206 32 35.59712974939206 33 35.59712974939206 34 35.59712974939206
		 35 35.59712974939206 36 35.59712974939206 37 35.59712974939206 38 35.59712974939206
		 39 35.59712974939206 40 35.59712974939206;
createNode animCurveTL -n "spine2_jnt_translateY";
	rename -uid "81704064-4BBD-2001-A7FD-4F82F763DBAE";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.7784338876254895e-14 1 -1.7784338876254895e-14
		 2 -1.7784338876254895e-14 3 -1.7784338876254895e-14 4 -1.7784338876254895e-14 5 -1.7784338876254895e-14
		 6 -1.7784338876254895e-14 7 -1.7784338876254895e-14 8 -1.7784338876254895e-14 9 -1.7784338876254895e-14
		 10 -1.7784338876254895e-14 11 -1.7784338876254895e-14 12 -1.7784338876254895e-14
		 13 -1.7784338876254895e-14 14 -1.7784338876254895e-14 15 -1.7784338876254895e-14
		 16 -1.7784338876254895e-14 17 -1.7784338876254895e-14 18 -1.7784338876254895e-14
		 19 -1.7784338876254895e-14 20 -1.7784338876254895e-14 21 -1.7784338876254895e-14
		 22 -1.7784338876254895e-14 23 -1.7784338876254895e-14 24 -1.7784338876254895e-14
		 25 -1.7784338876254895e-14 26 -1.7784338876254895e-14 27 -1.7784338876254895e-14
		 28 -1.7784338876254895e-14 29 -1.7784338876254895e-14 30 -1.7784338876254895e-14
		 31 -1.7784338876254895e-14 32 -1.7784338876254895e-14 33 -1.7784338876254895e-14
		 34 -1.7784338876254895e-14 35 -1.7784338876254895e-14 36 -1.7784338876254895e-14
		 37 -1.7784338876254895e-14 38 -1.7784338876254895e-14 39 -1.7784338876254895e-14
		 40 -1.7784338876254895e-14;
createNode animCurveTL -n "spine2_jnt_translateZ";
	rename -uid "828723EA-43C2-42F6-3F35-FFA9E39C1E32";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "spine2_jnt_rotateX";
	rename -uid "CFA2AF3A-42E6-E695-EDD4-919A166EDB85";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -8.5377364485770883e-07 1 -8.5377364485821705e-07
		 2 1.3940119463714355e-15 3 1.3866537049127527e-15 4 1.3853823355167579e-15 5 1.3780014298871971e-15
		 6 -8.5377364483279791e-07 7 -8.5377364485432133e-07 8 -8.5377364486155023e-07 9 -8.5377364486468626e-07
		 10 -8.5377364485404827e-07 11 -8.5377364485897779e-07 12 1.4002355313594194e-15 13 -8.537736448502668e-07
		 14 1.397495497071935e-15 15 1.3935918156004455e-15 16 -8.5377364484492022e-07 17 -8.537736448711685e-07
		 18 -8.5377364487031416e-07 19 1.3825197646490532e-15 20 1.3459689693620399e-15 21 -8.5377364485015912e-07
		 22 -8.5377364484534712e-07 23 1.4229455785753129e-15 24 -8.5377364484501403e-07 25 1.4195153934715148e-15
		 26 8.5377364760707031e-07 27 -8.5377364486693768e-07 28 1.3853217902335942e-15 29 -8.5377364482695889e-07
		 30 -8.5377364480353228e-07 31 1.4100639007924526e-15 32 1.3908148327585741e-15 33 1.4219501844140826e-15
		 34 -8.5377364489218762e-07 35 1.3865454074410921e-15 36 1.3664138183792793e-15 37 1.4083964683487055e-15
		 38 1.3955474520568888e-15 39 1.3975204785170827e-15 40 -8.5377364485770883e-07;
createNode animCurveTA -n "spine2_jnt_rotateY";
	rename -uid "F040B2EF-42DA-31F9-8922-9396616E9D77";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -4.2126990901182078e-15 1 -4.1876584361315762e-15
		 2 -4.1531299747911022e-15 3 -4.1453135801788873e-15 4 -4.214427223281744e-15 5 -4.2078392734226256e-15
		 6 -4.437134194208354e-15 7 -4.7915365229877014e-15 8 -3.5532080087693497e-15 9 -2.9080348524412685e-15
		 10 -3.9610328366211343e-15 11 -4.2290125555130704e-15 12 -3.8148081710716254e-15
		 13 -3.7554728383933228e-15 14 -4.3715882525366335e-15 15 -4.2269507614723612e-15
		 16 -3.8726505991313082e-15 17 -5.7073712712223771e-15 18 -5.0491776468675731e-15
		 19 -4.3700412536926706e-15 20 -5.9491742879033186e-15 21 -4.5652676311399819e-15
		 22 -5.1193171294174817e-15 23 -5.0272063364195423e-15 24 -5.9168790558177326e-15
		 25 -4.398025615816102e-15 26 -9.5105902580361353e-15 27 -1.3991208847098402e-15 28 -2.9942431781225455e-15
		 29 -7.3823229949887649e-15 30 -2.8462052250617645e-15 31 -4.9118277364328037e-15
		 32 -2.6956312946004527e-15 33 -4.2656693871534134e-15 34 -3.9952288717615243e-15
		 35 -4.5590242024784879e-15 36 -5.6534113027279491e-15 37 -2.9633427086715729e-15
		 38 -4.4774866891053417e-15 39 -4.3447407622391568e-15 40 -4.2126990901182078e-15;
createNode animCurveTA -n "spine2_jnt_rotateZ";
	rename -uid "62C3B819-4379-6B7B-FF10-28805DC9E010";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 2.1077136845887687 1 2.1077136845896334
		 2 2.1077136845887687 3 2.1077136845898061 4 2.1077136845887687 5 2.1077136845896334
		 6 2.1077136845887687 7 2.1077136845889406 8 2.1077136845887687 9 2.1077136845887687
		 10 2.1077136845889406 11 2.1077136845889406 12 2.1077136845889406 13 2.1077136845896334
		 14 2.1077136845896338 15 2.1077136845887687 16 2.1077136845887687 17 2.1077136845898061
		 18 2.1077136845887687 19 2.1077136845896334 20 2.1077136845887687 21 2.1077136845889406
		 22 2.1077136845887687 23 2.1077136845896329 24 2.1077136845887687 25 2.1077136845898061
		 26 2.1077136845896334 27 2.1077136845898057 28 2.1077136845887687 29 2.1077136845896334
		 30 2.1077136845887687 31 2.1077136845887687 32 2.1077136845898061 33 2.1077136845887687
		 34 2.1077136845887687 35 2.1077136845896334 36 2.1077136845887687 37 2.1077136845896334
		 38 2.1077136845887687 39 2.1077136845896334 40 2.1077136845887687;
createNode animCurveTU -n "spine2_jnt_scaleX";
	rename -uid "74B681C8-4258-1FA9-CF43-88AE3FB83761";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "spine2_jnt_scaleY";
	rename -uid "79A8B0F1-4C73-EF8B-68CC-85854B578D10";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "spine2_jnt_scaleZ";
	rename -uid "D6522595-4B9D-FC1B-AA41-5C8CE678D879";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "tail_jnt_visibility";
	rename -uid "D5B7AC9E-4069-7353-0F85-77B4ED12996C";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "tail_jnt_translateX";
	rename -uid "86E420C1-454F-147A-35A4-84BD5170EF9B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 22.50654114887676 1 22.50654114887676
		 2 22.50654114887676 3 22.50654114887676 4 22.50654114887676 5 22.50654114887676 6 22.50654114887676
		 7 22.50654114887676 8 22.50654114887676 9 22.50654114887676 10 22.50654114887676
		 11 22.50654114887676 12 22.50654114887676 13 22.50654114887676 14 22.50654114887676
		 15 22.50654114887676 16 22.50654114887676 17 22.50654114887676 18 22.50654114887676
		 19 22.50654114887676 20 22.50654114887676 21 22.50654114887676 22 22.50654114887676
		 23 22.50654114887676 24 22.50654114887676 25 22.50654114887676 26 22.50654114887676
		 27 22.50654114887676 28 22.50654114887676 29 22.50654114887676 30 22.50654114887676
		 31 22.50654114887676 32 22.50654114887676 33 22.50654114887676 34 22.50654114887676
		 35 22.50654114887676 36 22.50654114887676 37 22.50654114887676 38 22.50654114887676
		 39 22.50654114887676 40 22.50654114887676;
createNode animCurveTL -n "tail_jnt_translateY";
	rename -uid "FF11473C-4BF1-A5C6-7FBD-A593395DB8C5";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 2.4205766790464606e-14 1 2.4205766790464606e-14
		 2 2.4205766790464606e-14 3 2.4205766790464606e-14 4 2.4205766790464606e-14 5 2.4205766790464606e-14
		 6 2.4205766790464606e-14 7 2.4205766790464606e-14 8 2.4205766790464606e-14 9 2.4205766790464606e-14
		 10 2.4205766790464606e-14 11 2.4205766790464606e-14 12 2.4205766790464606e-14 13 2.4205766790464606e-14
		 14 2.4205766790464606e-14 15 2.4205766790464606e-14 16 2.4205766790464606e-14 17 2.4205766790464606e-14
		 18 2.4205766790464606e-14 19 2.4205766790464606e-14 20 2.4205766790464606e-14 21 2.4205766790464606e-14
		 22 2.4205766790464606e-14 23 2.4205766790464606e-14 24 2.4205766790464606e-14 25 2.4205766790464606e-14
		 26 2.4205766790464606e-14 27 2.4205766790464606e-14 28 2.4205766790464606e-14 29 2.4205766790464606e-14
		 30 2.4205766790464606e-14 31 2.4205766790464606e-14 32 2.4205766790464606e-14 33 2.4205766790464606e-14
		 34 2.4205766790464606e-14 35 2.4205766790464606e-14 36 2.4205766790464606e-14 37 2.4205766790464606e-14
		 38 2.4205766790464606e-14 39 2.4205766790464606e-14 40 2.4205766790464606e-14;
createNode animCurveTL -n "tail_jnt_translateZ";
	rename -uid "8AA17E85-4421-4F68-2965-0D979B0DF878";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -5.1214171332342635e-15 1 -5.1214171332342635e-15
		 2 -5.1214171332342635e-15 3 -5.1214171332342635e-15 4 -5.1214171332342635e-15 5 -5.1214171332342635e-15
		 6 -5.1214171332342635e-15 7 -5.1214171332342635e-15 8 -5.1214171332342635e-15 9 -5.1214171332342635e-15
		 10 -5.1214171332342635e-15 11 -5.1214171332342635e-15 12 -5.1214171332342635e-15
		 13 -5.1214171332342635e-15 14 -5.1214171332342635e-15 15 -5.1214171332342635e-15
		 16 -5.1214171332342635e-15 17 -5.1214171332342635e-15 18 -5.1214171332342635e-15
		 19 -5.1214171332342635e-15 20 -5.1214171332342635e-15 21 -5.1214171332342635e-15
		 22 -5.1214171332342635e-15 23 -5.1214171332342635e-15 24 -5.1214171332342635e-15
		 25 -5.1214171332342635e-15 26 -5.1214171332342635e-15 27 -5.1214171332342635e-15
		 28 -5.1214171332342635e-15 29 -5.1214171332342635e-15 30 -5.1214171332342635e-15
		 31 -5.1214171332342635e-15 32 -5.1214171332342635e-15 33 -5.1214171332342635e-15
		 34 -5.1214171332342635e-15 35 -5.1214171332342635e-15 36 -5.1214171332342635e-15
		 37 -5.1214171332342635e-15 38 -5.1214171332342635e-15 39 -5.1214171332342635e-15
		 40 -5.1214171332342635e-15;
createNode animCurveTA -n "tail_jnt_rotateX";
	rename -uid "802C555E-4B98-AD39-5CEE-BA82F74CA71E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "tail_jnt_rotateY";
	rename -uid "4CC63568-4F9B-4250-ECDB-48B1626B126F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "tail_jnt_rotateZ";
	rename -uid "D34731B9-4B68-F764-E64A-B7B99D438525";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "tail_jnt_scaleX";
	rename -uid "FE252DE1-47DA-0A89-D83D-1EA869216291";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "tail_jnt_scaleY";
	rename -uid "9323696E-4D74-D41A-BB17-5986FB24ED27";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "tail_jnt_scaleZ";
	rename -uid "0C3B06EA-403A-9C0D-A694-ABA28128C054";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "tailbase_jnt_visibility";
	rename -uid "65207EB7-4444-0754-936F-27A54ED43D5B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "tailbase_jnt_translateX";
	rename -uid "72284C89-4FD8-30E2-3417-C485E2B624CB";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 20.966794629543926 1 20.966794629543827
		 2 20.966794629543898 3 20.966794629543841 4 20.966794629543912 5 20.966794629543763
		 6 20.966794629543926 7 20.966794629543912 8 20.966794629543905 9 20.966794629543905
		 10 20.966794629543948 11 20.966794629543934 12 20.966794629543955 13 20.966794629543799
		 14 20.966794629543749 15 20.966794629543934 16 20.966794629543912 17 20.966794629543791
		 18 20.966794629543934 19 20.966794629543806 20 20.966794629543898 21 20.966794629543923
		 22 20.966794629543884 23 20.966794629543791 24 20.966794629543912 25 20.966794629543841
		 26 20.966794629543784 27 20.966794629543827 28 20.966794629543891 29 20.966794629543834
		 30 20.966794629543934 31 20.966794629543912 32 20.966794629543834 33 20.966794629543948
		 34 20.966794629543891 35 20.966794629543827 36 20.966794629543919 37 20.966794629543735
		 38 20.966794629543948 39 20.966794629543827 40 20.966794629543926;
createNode animCurveTL -n "tailbase_jnt_translateY";
	rename -uid "FDE9D05F-4FEF-1C11-1911-3081D4CF6F96";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -4.2632564145606011e-14 1 4.9737991503207013e-13
		 2 -2.8421709430404007e-14 3 5.1159076974727213e-13 4 5.6843418860808015e-14 5 6.1106675275368616e-13
		 6 -4.2632564145606011e-14 7 -8.5265128291212022e-14 8 2.8421709430404007e-14 9 -4.2632564145606011e-14
		 10 -8.5265128291212022e-14 11 -1.1368683772161603e-13 12 -5.6843418860808015e-14
		 13 6.6791017161449417e-13 14 6.9633188104489818e-13 15 -1.4210854715202004e-14 16 -5.6843418860808015e-14
		 17 5.4001247917767614e-13 18 -4.2632564145606011e-14 19 5.2580162446247414e-13 20 -1.4210854715202004e-14
		 21 -8.5265128291212022e-14 22 4.2632564145606011e-14 23 5.2580162446247414e-13 24 9.9475983006414026e-14
		 25 4.6895820560166612e-13 26 5.5422333389287814e-13 27 4.8316906031686813e-13 28 -2.8421709430404007e-14
		 29 5.2580162446247414e-13 30 2.8421709430404007e-14 31 5.6843418860808015e-14 32 4.2632564145606011e-13
		 33 5.6843418860808015e-14 34 -8.5265128291212022e-14 35 4.8316906031686813e-13 36 -2.8421709430404007e-14
		 37 6.6791017161449417e-13 38 -1.4210854715202004e-14 39 5.6843418860808015e-13 40 -4.2632564145606011e-14;
createNode animCurveTL -n "tailbase_jnt_translateZ";
	rename -uid "08ED7995-4E82-35D3-78DD-5CBC28593013";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -7.7461497324647698e-15 1 -7.7576833845682813e-15
		 2 -7.5495165674510645e-15 3 -7.5495165674510645e-15 4 -7.4384942649885488e-15 5 -7.7715611723760958e-15
		 6 -8.659739592076221e-15 7 -7.1054273576010019e-15 8 -7.7715611723760958e-15 9 -8.2156503822261584e-15
		 10 -8.4376949871511897e-15 11 -7.7715611723760958e-15 12 -7.3274719625260332e-15
		 13 -8.7707618945387367e-15 14 -7.6605388699135801e-15 15 -7.7299278089526524e-15
		 16 -8.1254447614753644e-15 17 -7.1886940844478886e-15 18 -7.1609385088322597e-15
		 19 -4.2188474935755949e-15 20 -6.6335825721353103e-15 21 -7.7715611723760958e-15
		 22 -7.7715611723760958e-15 23 -8.4376949871511897e-15 24 -5.5511151231257827e-15
		 25 -8.8817841970012523e-15 26 -2.2204460492503131e-15 27 -8.8817841970012523e-15
		 28 -8.8817841970012523e-15 29 -6.2172489379008766e-15 30 -3.5527136788005009e-15
		 31 -1.7763568394002505e-15 32 -7.1054273576010019e-15 33 -6.2172489379008766e-15
		 34 -7.7715611723760958e-15 35 -8.4376949871511897e-15 36 -8.4376949871511897e-15
		 37 -9.7699626167013776e-15 38 -7.5495165674510645e-15 39 -7.688294445529209e-15 40 -7.7461497324647698e-15;
createNode animCurveTA -n "tailbase_jnt_rotateX";
	rename -uid "F1F3959E-4A73-0641-E60E-109A7EE8BF4A";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 4.0662147897126559e-16 1 4.4966210647657634e-16
		 2 4.0662147897126559e-16 3 -2.0659501202549107e-16 4 2.6889147095427148e-16 5 3.2054022396064508e-16
		 6 4.0662147897126559e-16 7 3.1193209845958218e-16 8 4.1522960447232775e-16 9 4.1522960447232775e-16
		 10 3.0332397295852002e-16 11 5.1648753006372769e-17 12 -2.3241938852867674e-16 13 2.7863956924531985e-17
		 14 2.7863956924531991e-17 15 4.1522960447232775e-16 16 4.1522960447232775e-16 17 -1.8077063552230536e-16
		 18 4.1522960447232775e-16 19 3.0332397295852002e-16 20 1.3116146293727899e-16 21 3.1193209845958218e-16
		 22 2.6028334545320942e-16 23 6.0256878507434861e-17 24 1.3116146293727899e-16 25 3.5497272596489317e-16
		 26 3.0332397295852002e-16 27 -1.9798688652442817e-16 28 4.2383772997338986e-16 29 4.4966210647657634e-16
		 30 1.3976958843834041e-16 31 1.3976958843834038e-16 32 3.3775647496276895e-16 33 1.3116146293727901e-16
		 34 1.5494625901911829e-16 35 1.721625100212411e-16 36 3.9801335347020353e-16 37 3.647208242559192e-17
		 38 4.3244585547445202e-16 39 4.4105398097551418e-16 40 4.0662147897126559e-16;
createNode animCurveTA -n "tailbase_jnt_rotateY";
	rename -uid "0730408D-4B22-6C8C-D090-328D59E7F6C8";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 13.951365162496382 1 13.951365162496382
		 2 13.951365162496382 3 13.951365162496382 4 13.951365162496382 5 13.951365162496383
		 6 13.951365162496382 7 13.951365162496382 8 13.951365162496382 9 13.951365162496382
		 10 13.951365162496382 11 13.951365162496382 12 13.951365162496382 13 13.951365162496382
		 14 13.951365162496382 15 13.951365162496382 16 13.951365162496382 17 13.951365162496382
		 18 13.951365162496382 19 13.951365162496382 20 13.951365162496383 21 13.951365162496382
		 22 13.951365162496382 23 13.951365162496382 24 13.951365162496383 25 13.951365162496383
		 26 13.951365162496382 27 13.951365162496382 28 13.951365162496382 29 13.951365162496382
		 30 13.951365162496383 31 13.951365162496383 32 13.951365162496383 33 13.951365162496383
		 34 13.951365162496382 35 13.951365162496383 36 13.951365162496382 37 13.951365162496382
		 38 13.951365162496382 39 13.951365162496382 40 13.951365162496382;
createNode animCurveTA -n "tailbase_jnt_rotateZ";
	rename -uid "DD5C5109-442A-5B78-17DA-A8AB2B008BD1";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 4.9751669183775441e-17 1 -1.5966349143747971e-14
		 2 4.9751669183776236e-17 3 7.6877283902072403e-14 4 5.1301274270565366e-14 5 3.2081953295047272e-14
		 6 4.9751669183776236e-17 7 3.5285173457633614e-14 8 -3.1534684934025723e-15 9 -3.1534684934025746e-15
		 10 3.848839362021997e-14 11 -1.9219320975518104e-14 12 8.6486944389831419e-14 13 1.4099143882298316e-13
		 14 1.4099143882298316e-13 15 -3.1534684934025723e-15 16 -3.1534684934025746e-15 17 6.7267623414313337e-14
		 18 -3.1534684934025723e-15 19 3.848839362021997e-14 20 1.0255279687194696e-13 21 3.5285173457633608e-14
		 22 5.4504494433151715e-14 23 -2.242254113810445e-14 24 1.0255279687194693e-13 25 1.9269072644701869e-14
		 26 3.848839362021997e-14 27 7.3674063739486035e-14 28 -6.3566886559889206e-15 29 -1.5966349143747971e-14
		 30 9.9349576709360603e-14 31 9.9349576709360603e-14 32 2.5675512969874564e-14 33 1.0255279687194696e-13
		 34 -5.7657962926554296e-14 35 -6.4064403251726994e-14 36 3.2529718317701247e-15 37 1.3778821866039678e-13
		 38 -9.5599088185752697e-15 39 -1.2763128981161623e-14 40 4.9751669183776236e-17;
createNode animCurveTU -n "tailbase_jnt_scaleX";
	rename -uid "69890AD6-4865-3CB5-FCED-6C804135B37E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "tailbase_jnt_scaleY";
	rename -uid "E9C757AE-454D-5FA0-1499-50ABF60B37F3";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "tailbase_jnt_scaleZ";
	rename -uid "0DF4E1BB-42ED-BBC4-4E1D-2C9456E86737";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "tonguebase_jnt_visibility";
	rename -uid "8CC2034F-4143-E1B3-66BF-E0A8DD1A57EF";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "tonguebase_jnt_translateX";
	rename -uid "92213527-405B-2CDF-15D5-348FD04E3855";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 31.965998727681097 1 31.965998727681075
		 2 31.965998727681097 3 31.965998727681111 4 31.965998727681068 5 31.965998727681111
		 6 31.965998727681132 7 31.965998727681125 8 31.965998727681104 9 31.965998727681111
		 10 31.965998727681097 11 31.96599872768109 12 31.965998727681097 13 31.965998727681125
		 14 31.965998727681111 15 31.965998727681125 16 31.965998727681125 17 31.965998727681082
		 18 31.965998727681125 19 31.965998727681068 20 31.965998727681097 21 31.965998727681111
		 22 31.965998727681082 23 31.965998727681125 24 31.965998727681082 25 31.965998727681111
		 26 31.965998727681097 27 31.965998727681132 28 31.96599872768109 29 31.965998727681118
		 30 31.965998727681097 31 31.965998727681125 32 31.965998727681125 33 31.965998727681111
		 34 31.965998727681097 35 31.965998727681104 36 31.965998727681111 37 31.965998727681139
		 38 31.965998727681111 39 31.96599872768109 40 31.965998727681111;
createNode animCurveTL -n "tonguebase_jnt_translateY";
	rename -uid "A5ECD536-4596-66D6-E41D-9FAC1B8D7FFD";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.4210854715202004e-14 1 -1.4210854715202004e-14
		 2 -1.4210854715202004e-14 3 0 4 1.4210854715202004e-14 5 1.4210854715202004e-14 6 -5.6843418860808015e-14
		 7 0 8 -1.4210854715202004e-14 9 1.4210854715202004e-14 10 0 11 0 12 -2.8421709430404007e-14
		 13 -1.4210854715202004e-14 14 -1.4210854715202004e-14 15 -4.2632564145606011e-14
		 16 0 17 -4.2632564145606011e-14 18 -2.8421709430404007e-14 19 1.4210854715202004e-14
		 20 -1.4210854715202004e-14 21 1.4210854715202004e-14 22 0 23 -1.4210854715202004e-14
		 24 -1.4210854715202004e-14 25 -2.8421709430404007e-14 26 -2.8421709430404007e-14
		 27 0 28 0 29 0 30 -1.4210854715202004e-14 31 -4.2632564145606011e-14 32 -1.4210854715202004e-14
		 33 0 34 -2.8421709430404007e-14 35 0 36 -1.4210854715202004e-14 37 1.4210854715202004e-14
		 38 -1.4210854715202004e-14 39 1.4210854715202004e-14 40 -4.2632564145606011e-14;
createNode animCurveTL -n "tonguebase_jnt_translateZ";
	rename -uid "E27E62E9-4401-B26D-3441-D6B82B689376";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.0894744347362186e-14 1 2.5098451095217911e-11
		 2 -2.2648549702353193e-14 3 -2.3203661214665772e-14 4 -2.3481216970822061e-14 5 -2.5091040356528538e-14
		 6 -2.8421709430404007e-14 7 -1.9539925233402755e-14 8 -2.6645352591003757e-14 9 -3.3750779948604759e-14
		 10 -8.8817841970012523e-15 11 -2.3092638912203256e-14 12 -2.6645352591003757e-14
		 13 -2.6645352591003757e-14 14 -2.1316282072803006e-14 15 -2.3092638912203256e-14
		 16 -2.042810365310288e-14 17 -2.1527585269964788e-11 18 -2.042810365310288e-14 19 -2.1316282072803006e-14
		 20 -1.6875389974302379e-14 21 -2.6645352591003757e-14 22 -2.1316282072803006e-14
		 23 -1.9539925233402755e-14 24 -2.4868995751603507e-14 25 -2.6645352591003757e-14
		 26 -2.042810365310288e-14 27 -2.1760371282653068e-14 28 -2.2204460492503131e-14 29 -2.2870594307278225e-14
		 30 -2.3980817331903381e-14 31 -2.3092638912203256e-14 32 -2.3536728122053319e-14
		 33 -2.2995494397548555e-14 34 -2.3314683517128287e-14 35 -2.2204460492503131e-14
		 36 -2.2204460492503131e-14 37 -2.2204460492503131e-14 38 -2.2648549702353193e-14
		 39 -2.248201624865942e-14 40 -1.0890081392581277e-14;
createNode animCurveTA -n "tonguebase_jnt_rotateX";
	rename -uid "E245C79E-4F54-F956-DDA5-439518D10AB3";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 2.5444437451708134e-14 1 1.673226206824327e-10
		 2 2.5444437451708134e-14 3 2.5444437404314176e-14 4 2.5444437451708134e-14 5 2.5444437451708134e-14
		 6 2.5444437451708134e-14 7 2.5444437451708134e-14 8 2.5444437404314176e-14 9 2.5444437404314176e-14
		 10 2.5444437451708134e-14 11 2.5444437451708134e-14 12 2.5444437451708134e-14 13 2.5444437451708134e-14
		 14 2.5444437404314176e-14 15 2.5444437451708134e-14 16 2.5444437404314176e-14 17 -1.0717197054664205e-10
		 18 2.5444437451708134e-14 19 2.5444437451708134e-14 20 2.5444437451708134e-14 21 2.5444437451708134e-14
		 22 2.5444437404314176e-14 23 2.5444437451708134e-14 24 2.5444437404314176e-14 25 2.5444437451708134e-14
		 26 2.5444437404314176e-14 27 2.5444437451708134e-14 28 2.5444437404314176e-14 29 2.5444437451708134e-14
		 30 2.5444437451708134e-14 31 2.5444437451708134e-14 32 2.5444437451708134e-14 33 2.5444437451708134e-14
		 34 2.5444437404314176e-14 35 2.5444437404314176e-14 36 2.5444437451708134e-14 37 2.5444437451708134e-14
		 38 2.5444437451708134e-14 39 2.5444437451708134e-14 40 2.5444437451708134e-14;
createNode animCurveTA -n "tonguebase_jnt_rotateY";
	rename -uid "34712FBA-4E4D-CC6C-31A3-F082B3FAC789";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.1848489498583662e-23 1 1.0667355857245742e-22
		 2 1.1848489498583662e-23 3 1.0663641113705303e-22 4 1.1848489498583662e-23 5 1.1848489498583662e-23
		 6 -8.2939430727435685e-23 7 1.1848489498583662e-23 8 1.0663641113705303e-22 9 2.0142433277552239e-22
		 10 1.1848489498583662e-23 11 -8.2939430727435685e-23 12 1.1848489498583662e-23 13 1.1848489498583662e-23
		 14 1.0663641113705303e-22 15 1.1848489498583662e-23 16 1.0663641113705303e-22 17 0
		 18 1.1848489498583662e-23 19 1.1848489498583662e-23 20 1.1848489498583662e-23 21 1.1848489498583662e-23
		 22 1.0663641113705303e-22 23 1.1848489498583662e-23 24 1.0663641113705303e-22 25 1.1848489498583662e-23
		 26 1.0663641113705303e-22 27 -8.2939430727435685e-23 28 1.0663641113705303e-22 29 -8.2939430727435685e-23
		 30 1.1848489498583662e-23 31 1.1848489498583662e-23 32 1.1848489498583662e-23 33 -8.2939430727435685e-23
		 34 1.0663641113705303e-22 35 1.0663641113705303e-22 36 1.1848489498583662e-23 37 1.1848489498583662e-23
		 38 1.1848489498583662e-23 39 -8.2939430727435685e-23 40 1.1848489498583662e-23;
createNode animCurveTA -n "tonguebase_jnt_rotateZ";
	rename -uid "8EAAE61C-445C-3D69-52A6-07A696D367A2";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 2.6308931696713916e-39 1 -2.5444437451708134e-14
		 2 0 3 -2.5444437451708134e-14 4 0 5 2.6308931696713916e-39 6 1.9083328088781101e-14
		 7 0 8 -2.5444437451708134e-14 9 -5.0888874903416268e-14 10 0 11 1.9083328088781101e-14
		 12 0 13 2.6308931696713916e-39 14 -2.5444437451708134e-14 15 0 16 -2.5444437451708134e-14
		 17 0 18 2.6308931696713916e-39 19 2.6308931696713916e-39 20 2.6308931696713916e-39
		 21 2.6308931696713916e-39 22 -2.5444437451708134e-14 23 0 24 -2.5444437451708134e-14
		 25 0 26 -2.5444437451708134e-14 27 1.9083328088781101e-14 28 -2.5444437451708134e-14
		 29 1.9083328088781101e-14 30 0 31 2.6308931696713916e-39 32 2.6308931696713916e-39
		 33 1.9083328088781101e-14 34 -2.5444437451708134e-14 35 -2.5444437451708134e-14 36 0
		 37 2.6308931696713916e-39 38 2.6308931696713916e-39 39 1.9083328088781101e-14 40 0;
createNode animCurveTU -n "tonguebase_jnt_scaleX";
	rename -uid "5F6FF640-4644-CA3A-C7B9-EBB955BC72BA";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.99999999999999978 1 0.99999999999999989
		 2 0.99999999999999978 3 1 4 0.99999999999999956 5 1.0000000000000002 6 1 7 0.99999999999999967
		 8 0.99999999999999944 9 0.99999999999999933 10 1 11 0.99999999999999944 12 0.99999999999999956
		 13 0.99999999999999978 14 0.99999999999999933 15 0.99999999999999967 16 0.99999999999999978
		 17 0.99999999999999967 18 0.99999999999999956 19 0.99999999999999933 20 0.99999999999999956
		 21 1 22 0.99999999999999933 23 1 24 0.99999999999999911 25 0.99999999999999967 26 0.99999999999999911
		 27 1 28 0.99999999999999944 29 0.99999999999999978 30 1 31 0.99999999999999944 32 0.99999999999999978
		 33 1 34 0.99999999999999967 35 0.99999999999999956 36 0.99999999999999944 37 0.99999999999999978
		 38 1 39 0.99999999999999978 40 0.99999999999999978;
createNode animCurveTU -n "tonguebase_jnt_scaleY";
	rename -uid "BCF1A08D-4264-01BF-6782-829E2EC329C9";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.0000000000000002 1 1 2 1.0000000000000002
		 3 1.0000000000000002 4 1.0000000000000002 5 1.0000000000000004 6 1.0000000000000007
		 7 1 8 0.99999999999999989 9 1.0000000000000002 10 1 11 0.99999999999999978 12 1.0000000000000002
		 13 1 14 1.0000000000000002 15 1 16 1.0000000000000004 17 1.0000000000000002 18 1.0000000000000002
		 19 1 20 0.99999999999999989 21 1 22 1.0000000000000002 23 1.0000000000000002 24 1
		 25 1 26 1 27 1 28 1 29 1 30 1.0000000000000004 31 1.0000000000000002 32 1 33 1 34 1.0000000000000002
		 35 1.0000000000000004 36 1.0000000000000004 37 1.0000000000000002 38 1 39 0.99999999999999989
		 40 1.0000000000000002;
createNode animCurveTU -n "tonguebase_jnt_scaleZ";
	rename -uid "8DC93262-4ADF-E4B6-A8AA-A596B31500B2";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.99999999999999989 1 0.99999999999999944
		 2 0.99999999999999989 3 0.99999999999999989 4 0.99999999999999956 5 1.0000000000000002
		 6 1.0000000000000002 7 0.99999999999999956 8 0.99999999999999978 9 0.99999999999999956
		 10 0.99999999999999967 11 0.99999999999999978 12 0.99999999999999978 13 1.0000000000000002
		 14 0.99999999999999967 15 1 16 0.99999999999999978 17 0.99999999999999944 18 0.99999999999999944
		 19 0.99999999999999956 20 1 21 0.99999999999999978 22 0.99999999999999956 23 1 24 0.99999999999999944
		 25 0.99999999999999956 26 0.99999999999999922 27 1 28 0.99999999999999989 29 1 30 1
		 31 0.99999999999999978 32 0.99999999999999956 33 1 34 0.99999999999999933 35 0.99999999999999978
		 36 0.99999999999999989 37 1 38 0.99999999999999978 39 1 40 0.99999999999999989;
createNode animCurveTU -n "tonguemid_jnt_visibility";
	rename -uid "02920E24-4504-5913-31ED-FC9CCD470B46";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "tonguemid_jnt_translateX";
	rename -uid "AFDA8832-4301-0781-6B6D-D5B7AEFCD2A7";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 24.87206729816269 1 24.872067298162669
		 2 24.872067298162687 3 24.872067298162669 4 24.872067298162669 5 24.872067298162687
		 6 24.872067298162683 7 24.872067298162701 8 24.872067298162683 9 24.872067298162705
		 10 24.872067298162676 11 24.872067298162694 12 24.87206729816268 13 24.872067298162687
		 14 24.872067298162694 15 24.872067298162701 16 24.872067298162708 17 24.872067298162683
		 18 24.872067298162683 19 24.872067298162701 20 24.872067298162673 21 24.872067298162712
		 22 24.872067298162694 23 24.87206729816269 24 24.872067298162705 25 24.872067298162705
		 26 24.872067298162705 27 24.872067298162662 28 24.872067298162673 29 24.872067298162705
		 30 24.872067298162676 31 24.872067298162694 32 24.872067298162705 33 24.872067298162683
		 34 24.872067298162694 35 24.872067298162698 36 24.87206729816268 37 24.872067298162705
		 38 24.87206729816268 39 24.87206729816269 40 24.872067298162694;
createNode animCurveTL -n "tonguemid_jnt_translateY";
	rename -uid "6364E196-4B10-37E5-05E0-39825A40FE23";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 5.6843418860808015e-14 1 4.2632564145606011e-14
		 2 9.9475983006414026e-14 3 5.6843418860808015e-14 4 0 5 4.2632564145606011e-14 6 5.6843418860808015e-14
		 7 5.6843418860808015e-14 8 8.5265128291212022e-14 9 2.8421709430404007e-14 10 5.6843418860808015e-14
		 11 4.2632564145606011e-14 12 5.6843418860808015e-14 13 5.6843418860808015e-14 14 7.1054273576010019e-14
		 15 9.9475983006414026e-14 16 2.8421709430404007e-14 17 8.5265128291212022e-14 18 4.2632564145606011e-14
		 19 4.2632564145606011e-14 20 5.6843418860808015e-14 21 4.2632564145606011e-14 22 4.2632564145606011e-14
		 23 2.8421709430404007e-14 24 4.2632564145606011e-14 25 7.1054273576010019e-14 26 2.8421709430404007e-14
		 27 5.6843418860808015e-14 28 4.2632564145606011e-14 29 4.2632564145606011e-14 30 8.5265128291212022e-14
		 31 8.5265128291212022e-14 32 7.1054273576010019e-14 33 8.5265128291212022e-14 34 7.1054273576010019e-14
		 35 8.5265128291212022e-14 36 4.2632564145606011e-14 37 1.4210854715202004e-14 38 5.6843418860808015e-14
		 39 2.8421709430404007e-14 40 7.1054273576010019e-14;
createNode animCurveTL -n "tonguemid_jnt_translateZ";
	rename -uid "3F6B033E-4980-D75D-07C8-91A56C547AA3";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -3.1977911277954287e-07 1 -3.1979901700396329e-07
		 2 -3.1977912351432991e-07 3 -3.1977912351432991e-07 4 -3.1977912362535221e-07 5 -3.1977912406944142e-07
		 6 -3.1977912406944142e-07 7 -3.197791187403709e-07 8 -3.197791276221551e-07 9 -3.197791276221551e-07
		 10 -3.1977911163494355e-07 11 -3.1977912406944142e-07 12 -3.1977913295122562e-07
		 13 -3.197791276221551e-07 14 -3.1977912229308458e-07 15 -3.1977912584579826e-07 16 -3.1977912229308458e-07
		 17 -3.1977125430904252e-07 18 -3.1977912229308458e-07 19 -3.1977912584579826e-07
		 20 -3.197791187403709e-07 21 -3.1977912406944142e-07 22 -3.1977912406944142e-07 23 -3.19779123181263e-07
		 24 -3.197791276221551e-07 25 -3.1977912229308458e-07 26 -3.1977912273717379e-07 27 -3.1977912251512919e-07
		 28 -3.1977912273717379e-07 29 -3.1977912273717379e-07 30 -3.1977912362535221e-07
		 31 -3.1977912406944142e-07 32 -3.1977912362535221e-07 33 -3.197791235282077e-07 34 -3.1977912362535221e-07
		 35 -3.1977912340330761e-07 36 -3.1977912273717379e-07 37 -3.197791229592184e-07 38 -3.1977912362535221e-07
		 39 -3.1977912356984106e-07 40 -3.1977911278428959e-07;
createNode animCurveTA -n "tonguemid_jnt_rotateX";
	rename -uid "121E76B0-4D40-24C0-C7EF-F6948C8CD434";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "tonguemid_jnt_rotateY";
	rename -uid "6794017A-4961-5E30-FE80-57BC83633D0E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "tonguemid_jnt_rotateZ";
	rename -uid "93EEBD5F-4A97-8F2B-74CA-458A0038EE96";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.5107634736951704e-14 1 -9.5416640443905503e-15
		 2 2.3059021440610497e-14 3 -1.033680271475643e-14 4 1.8288189418415221e-14 5 1.1927080055488187e-14
		 6 1.1927080055488187e-14 7 -3.1805546814635168e-15 8 -1.5107634736951704e-14 9 -2.8624992133171654e-14
		 10 2.3854160110976376e-15 11 1.7493050748049341e-14 12 1.6697912077683464e-14 13 1.5902773407317584e-15
		 14 -7.9513867036587919e-15 15 7.1562480332929135e-15 16 -7.9513867036587919e-16 17 1.033680271475643e-14
		 18 1.5107634736951704e-14 19 1.2722218725854067e-14 20 1.1927080055488187e-14 21 6.3611093629270335e-15
		 22 -7.1562480332929135e-15 23 1.5107634736951704e-14 24 7.9513867036587919e-16 25 1.8288189418415221e-14
		 26 -1.1927080055488187e-14 27 2.3854160110976374e-14 28 -1.5107634736951704e-14 29 2.5444437451708134e-14
		 30 1.2722218725854067e-14 31 1.1131941385122309e-14 32 1.4312496066585827e-14 33 2.1468744099878737e-14
		 34 -8.7465253740246703e-15 35 -6.3611093629270335e-15 36 3.1805546814635168e-15 37 4.7708320221952752e-15
		 38 1.1927080055488187e-14 39 2.6239576122074014e-14 40 1.5107634736951704e-14;
createNode animCurveTU -n "tonguemid_jnt_scaleX";
	rename -uid "69B75838-4BC8-F8DF-B838-17BE27572F22";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1.0000000000000004 2 1 3 0.99999999999999978
		 4 1 5 0.99999999999999978 6 0.99999999999999944 7 1.0000000000000002 8 0.99999999999999978
		 9 1 10 1 11 0.99999999999999989 12 0.99999999999999967 13 0.99999999999999944 14 0.99999999999999978
		 15 0.99999999999999978 16 1 17 1.0000000000000002 18 1 19 1 20 0.99999999999999978
		 21 1 22 0.99999999999999978 23 0.99999999999999956 24 0.99999999999999989 25 1.0000000000000002
		 26 1 27 0.99999999999999978 28 0.99999999999999956 29 0.99999999999999978 30 0.99999999999999978
		 31 0.99999999999999978 32 1.0000000000000002 33 0.99999999999999978 34 1.0000000000000004
		 35 0.99999999999999956 36 0.99999999999999956 37 0.99999999999999978 38 1.0000000000000004
		 39 0.99999999999999978 40 1;
createNode animCurveTU -n "tonguemid_jnt_scaleY";
	rename -uid "B79C8F67-4780-29A6-B72A-969664CE3709";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.99999999999999978 1 1 2 0.99999999999999989
		 3 1 4 1 5 0.99999999999999967 6 0.99999999999999978 7 1 8 1.0000000000000002 9 1
		 10 1 11 1.0000000000000007 12 1 13 1.0000000000000002 14 1.0000000000000002 15 0.99999999999999989
		 16 1 17 1 18 0.99999999999999989 19 1.0000000000000002 20 1.0000000000000002 21 1
		 22 1.0000000000000002 23 0.99999999999999967 24 1.0000000000000002 25 1.0000000000000002
		 26 1.0000000000000002 27 1 28 1.0000000000000002 29 1.0000000000000002 30 1 31 1
		 32 1.0000000000000004 33 1 34 1.0000000000000002 35 0.99999999999999967 36 1 37 0.99999999999999989
		 38 1.0000000000000002 39 1 40 0.99999999999999978;
createNode animCurveTU -n "tonguemid_jnt_scaleZ";
	rename -uid "973D6684-4E0C-22E2-4C0E-7CBD3CCC9B09";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 0.99999999999999944 2 1 3 0.99999999999999956
		 4 0.99999999999999989 5 0.99999999999999978 6 0.99999999999999978 7 0.99999999999999989
		 8 1.0000000000000004 9 1.0000000000000002 10 0.99999999999999967 11 1.0000000000000007
		 12 1 13 1.0000000000000002 14 1.0000000000000002 15 1.0000000000000002 16 1 17 0.99999999999999989
		 18 0.99999999999999989 19 1.0000000000000002 20 1.0000000000000004 21 0.99999999999999967
		 22 1.0000000000000002 23 0.99999999999999989 24 1.0000000000000002 25 1 26 1 27 0.99999999999999978
		 28 1.0000000000000002 29 1.0000000000000002 30 0.99999999999999978 31 1 32 1 33 0.99999999999999989
		 34 0.99999999999999978 35 0.99999999999999989 36 1.0000000000000002 37 1.0000000000000002
		 38 0.99999999999999978 39 1.0000000000000004 40 1;
createNode animCurveTU -n "tonguetip_jnt_visibility";
	rename -uid "E8ACF089-4406-24CE-CEDA-A6B438248F7B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "tonguetip_jnt_translateX";
	rename -uid "4166DC98-4AEF-4C66-D03D-2D9C21ABC089";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 24.30264381865863 1 24.302643818658616
		 2 24.302643818658645 3 24.302643818658652 4 24.302643818658638 5 24.302643818658627
		 6 24.302643818658623 7 24.30264381865863 8 24.302643818658648 9 24.302643818658645
		 10 24.302643818658645 11 24.302643818658645 12 24.302643818658638 13 24.302643818658666
		 14 24.302643818658638 15 24.302643818658645 16 24.30264381865863 17 24.30264381865863
		 18 24.302643818658623 19 24.302643818658609 20 24.302643818658645 21 24.302643818658616
		 22 24.302643818658609 23 24.302643818658616 24 24.302643818658616 25 24.302643818658595
		 26 24.302643818658645 27 24.302643818658652 28 24.302643818658652 29 24.302643818658645
		 30 24.302643818658648 31 24.302643818658645 32 24.302643818658623 33 24.302643818658652
		 34 24.302643818658602 35 24.302643818658652 36 24.302643818658673 37 24.302643818658659
		 38 24.302643818658609 39 24.302643818658659 40 24.30264381865863;
createNode animCurveTL -n "tonguetip_jnt_translateY";
	rename -uid "9B37F851-4C14-BFEA-5070-53A7DCA73E8E";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 5.6843418860808015e-14 1 7.1054273576010019e-14
		 2 5.6843418860808015e-14 3 0 4 8.5265128291212022e-14 5 4.2632564145606011e-14 6 9.9475983006414026e-14
		 7 5.6843418860808015e-14 8 4.2632564145606011e-14 9 1.4210854715202004e-14 10 5.6843418860808015e-14
		 11 8.5265128291212022e-14 12 8.5265128291212022e-14 13 5.6843418860808015e-14 14 2.8421709430404007e-14
		 15 2.8421709430404007e-14 16 7.1054273576010019e-14 17 2.8421709430404007e-14 18 5.6843418860808015e-14
		 19 8.5265128291212022e-14 20 5.6843418860808015e-14 21 2.8421709430404007e-14 22 7.1054273576010019e-14
		 23 8.5265128291212022e-14 24 5.6843418860808015e-14 25 8.5265128291212022e-14 26 5.6843418860808015e-14
		 27 7.1054273576010019e-14 28 4.2632564145606011e-14 29 5.6843418860808015e-14 30 4.2632564145606011e-14
		 31 5.6843418860808015e-14 32 2.8421709430404007e-14 33 5.6843418860808015e-14 34 5.6843418860808015e-14
		 35 4.2632564145606011e-14 36 9.9475983006414026e-14 37 9.9475983006414026e-14 38 7.1054273576010019e-14
		 39 1.1368683772161603e-13 40 7.1054273576010019e-14;
createNode animCurveTL -n "tonguetip_jnt_translateZ";
	rename -uid "3FE755B1-4074-787B-125D-CAA55525184C";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 3.7085141787728607e-07 1 3.7083770804136051e-07
		 2 3.7085140736081712e-07 3 3.7085140713877252e-07 4 3.7085140736081712e-07 5 3.7085140824899554e-07
		 6 3.7085140736081712e-07 7 3.7085141357806606e-07 8 3.7085140824899554e-07 9 3.7085140469628186e-07
		 10 3.7085140469628186e-07 11 3.7085141357806606e-07 12 3.7085140291992502e-07 13 3.7085141002535238e-07
		 14 3.7085141357806606e-07 15 3.7085140469628186e-07 16 3.7085141002535238e-07 17 3.7085543480586125e-07
		 18 3.7085140824899554e-07 19 3.7085140736081712e-07 20 3.7085141002535238e-07 21 3.7085140291992502e-07
		 22 3.708514064726387e-07 23 3.7085140469628186e-07 24 3.7085140291992502e-07 25 3.7085140558446028e-07
		 26 3.7085141002535238e-07 27 3.708514062505941e-07 28 3.7085140780490633e-07 29 3.7085140758286173e-07
		 30 3.7085140780490633e-07 31 3.7085140780490633e-07 32 3.7085140691672791e-07 33 3.7085140736081712e-07
		 34 3.7085140780490633e-07 35 3.7085140824899554e-07 36 3.7085140736081712e-07 37 3.7085140736081712e-07
		 38 3.7085140758286173e-07 39 3.7085140730530597e-07 40 3.7085141787385675e-07;
createNode animCurveTA -n "tonguetip_jnt_rotateX";
	rename -uid "06A4B11B-4F92-D7B8-D844-649550868C7B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "tonguetip_jnt_rotateY";
	rename -uid "C2D94F9C-4482-56DC-6D67-44BF6F8E0557";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "tonguetip_jnt_rotateZ";
	rename -uid "9B201856-43D2-985A-FC80-CFB9D6B3B0EB";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -25.614289791145893 1 -25.614289791145893
		 2 -25.614289791145893 3 -25.614289791145893 4 -25.614289791145893 5 -25.614289791145893
		 6 -25.614289791145893 7 -25.614289791145893 8 -25.614289791145893 9 -25.614289791145893
		 10 -25.614289791145893 11 -25.614289791145893 12 -25.614289791145893 13 -25.614289791145893
		 14 -25.614289791145893 15 -25.614289791145893 16 -25.614289791145893 17 -25.614289791145893
		 18 -25.614289791145893 19 -25.614289791145893 20 -25.614289791145893 21 -25.614289791145893
		 22 -25.614289791145893 23 -25.614289791145893 24 -25.614289791145893 25 -25.614289791145893
		 26 -25.614289791145893 27 -25.614289791145893 28 -25.614289791145893 29 -25.614289791145893
		 30 -25.614289791145893 31 -25.614289791145893 32 -25.614289791145893 33 -25.614289791145893
		 34 -25.614289791145893 35 -25.614289791145893 36 -25.614289791145893 37 -25.614289791145893
		 38 -25.614289791145893 39 -25.614289791145893 40 -25.614289791145893;
createNode animCurveTU -n "tonguetip_jnt_scaleX";
	rename -uid "8C58B440-4302-950B-CDA1-0BA8C173081D";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1.0000000000000011 2 1 3 1.0000000000000002
		 4 1.0000000000000002 5 1 6 0.99999999999999978 7 1.0000000000000002 8 0.99999999999999933
		 9 0.99999999999999978 10 1.0000000000000004 11 0.99999999999999911 12 0.99999999999999967
		 13 0.99999999999999922 14 0.99999999999999956 15 0.99999999999999967 16 1 17 1.0000000000000002
		 18 1.0000000000000002 19 0.99999999999999967 20 0.99999999999999911 21 1.0000000000000004
		 22 0.99999999999999956 23 0.99999999999999978 24 0.99999999999999956 25 1 26 1 27 1.0000000000000002
		 28 0.99999999999999944 29 0.99999999999999978 30 1.0000000000000002 31 0.99999999999999967
		 32 1.0000000000000002 33 1 34 1.0000000000000004 35 0.99999999999999978 36 0.99999999999999944
		 37 0.99999999999999978 38 1.0000000000000007 39 0.99999999999999933 40 1;
createNode animCurveTU -n "tonguetip_jnt_scaleY";
	rename -uid "2393CF28-49D3-4105-8556-02BBAC12D0D8";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.0000000000000004 1 1.0000000000000002
		 2 1.0000000000000004 3 1 4 1.0000000000000004 5 1.0000000000000004 6 1.0000000000000004
		 7 1 8 1 9 1.0000000000000002 10 1.0000000000000004 11 0.99999999999999989 12 1 13 1
		 14 1.0000000000000002 15 1.0000000000000004 16 1.0000000000000002 17 1.0000000000000002
		 18 1.0000000000000004 19 1 20 1 21 1.0000000000000002 22 1.0000000000000002 23 1.0000000000000002
		 24 1 25 1 26 1.0000000000000002 27 1.0000000000000004 28 1.0000000000000002 29 1.0000000000000004
		 30 1.0000000000000004 31 1.0000000000000002 32 1 33 1.0000000000000002 34 1 35 1.0000000000000002
		 36 1.0000000000000004 37 1.0000000000000007 38 1.0000000000000002 39 1.0000000000000002
		 40 1.0000000000000004;
createNode animCurveTU -n "tonguetip_jnt_scaleZ";
	rename -uid "EF5D06B2-40E0-07F3-1A79-3FB6AD9263B3";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.0000000000000004 1 0.99999999999999944
		 2 1.0000000000000002 3 1 4 1.0000000000000002 5 1.0000000000000002 6 1.0000000000000007
		 7 0.99999999999999989 8 1.0000000000000004 9 1.0000000000000004 10 1 11 1.0000000000000009
		 12 1.0000000000000004 13 1.0000000000000011 14 1.0000000000000009 15 1.0000000000000007
		 16 1.0000000000000002 17 0.99999999999999967 18 1 19 1.0000000000000002 20 1.0000000000000009
		 21 1 22 1.0000000000000007 23 1.0000000000000007 24 1.0000000000000002 25 0.99999999999999989
		 26 1.0000000000000002 27 1.0000000000000002 28 1.0000000000000009 29 1.0000000000000007
		 30 1.0000000000000004 31 1.0000000000000004 32 0.99999999999999989 33 1.0000000000000004
		 34 0.99999999999999933 35 1.0000000000000007 36 1.0000000000000011 37 1.0000000000000009
		 38 0.99999999999999956 39 1.0000000000000009 40 1.0000000000000004;
createNode animCurveTU -n "torso_jnt_visibility";
	rename -uid "D19E892B-4ED3-7622-A73B-BEB39F4F9175";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "torso_jnt_translateX";
	rename -uid "0702236B-4F26-66CF-ABAA-DF9411A60196";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 3.1554436208840472e-30 1 1.3877787807814457e-17
		 2 -5.5511151231257827e-17 3 0 4 -3.3306690738754696e-16 5 -2.2204460492503131e-16
		 6 1.1102230246251565e-15 7 -2.2204460492503131e-16 8 -4.4408920985006262e-16 9 8.8817841970012523e-16
		 10 4.4408920985006262e-16 11 2.2204460492503131e-16 12 -2.2204460492503131e-16 13 5.5511151231257827e-16
		 14 -5.5511151231257827e-17 15 6.9388939039072284e-17 16 1.9428902930940239e-16 17 3.3306690738754696e-16
		 18 -8.3266726846886741e-16 19 -8.0491169285323849e-16 20 -1.2490009027033011e-15
		 21 -4.4408920985006262e-16 22 4.4408920985006262e-16 23 -4.4408920985006262e-16 24 -1.3322676295501878e-15
		 25 -4.4408920985006262e-16 26 0 27 0 28 8.8817841970012523e-16 29 0 30 -1.7763568394002505e-15
		 31 -3.5527136788005009e-15 32 -8.8817841970012523e-16 33 -4.4408920985006262e-16
		 34 -2.2204460492503131e-16 35 0 36 -7.7715611723760958e-16 37 0 38 -2.2204460492503131e-16
		 39 -6.9388939039072284e-18 40 3.1554436208840472e-30;
createNode animCurveTL -n "torso_jnt_translateY";
	rename -uid "4678B116-4304-F3BA-FC31-059AE420151A";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 14.000663064680438 1 14.000663064680481
		 2 14.00066306468041 3 14.000663064680467 4 14.000663064680424 5 14.000663064680424
		 6 14.000663064680481 7 14.000663064680424 8 14.000663064680381 9 14.000663064680438
		 10 14.000663064680452 11 14.000663064680424 12 14.000663064680452 13 14.000663064680467
		 14 14.000663064680424 15 14.00066306468041 16 14.000663064680452 17 14.000663064680424
		 18 14.000663064680452 19 14.000663064680467 20 14.000663064680509 21 14.000663064680452
		 22 14.000663064680396 23 14.000663064680424 24 14.000663064680438 25 14.000663064680467
		 26 14.000663064680452 27 14.000663064680452 28 14.000663064680438 29 14.000663064680452
		 30 14.000663064680509 31 14.000663064680452 32 14.000663064680467 33 14.000663064680481
		 34 14.00066306468041 35 14.000663064680452 36 14.000663064680438 37 14.000663064680424
		 38 14.000663064680452 39 14.00066306468041 40 14.000663064680438;
createNode animCurveTL -n "torso_jnt_translateZ";
	rename -uid "6FD3719F-4B25-BE04-363A-94A885C97AD8";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 16.516969193356793 1 16.516969193356807
		 2 16.516969193356793 3 16.5169691933568 4 16.5169691933568 5 16.516969193356797 6 16.516969193356793
		 7 16.516969193356793 8 16.5169691933568 9 16.5169691933568 10 16.5169691933568 11 16.516969193356786
		 12 16.5169691933568 13 16.5169691933568 14 16.516969193356807 15 16.516969193356797
		 16 16.516969193356793 17 16.516969193356783 18 16.516969193356804 19 16.5169691933568
		 20 16.5169691933568 21 16.51696919335679 22 16.516969193356786 23 16.516969193356786
		 24 16.516969193356807 25 16.516969193356797 26 16.516969193356786 27 16.516969193356786
		 28 16.516969193356793 29 16.516969193356793 30 16.516969193356804 31 16.516969193356786
		 32 16.516969193356797 33 16.5169691933568 34 16.516969193356793 35 16.516969193356793
		 36 16.5169691933568 37 16.516969193356793 38 16.516969193356797 39 16.516969193356793
		 40 16.516969193356793;
createNode animCurveTA -n "torso_jnt_rotateX";
	rename -uid "F777AB94-4979-C206-DC29-E0907B9DF42D";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1.4398804892874575e-16 1 1.4425831614542003e-16
		 2 1.441217867482381e-16 3 1.4535434353589799e-16 4 1.422683695199519e-16 5 1.4314930505595868e-16
		 6 1.4626124647054959e-16 7 1.4455941211097786e-16 8 1.5165520914949327e-16 9 1.4562912476940134e-16
		 10 1.4587992680036205e-16 11 1.4251718335514473e-16 12 1.465942533927798e-16 13 1.4270027566608529e-16
		 14 1.4152121944875365e-16 15 1.442424278261529e-16 16 1.450899702197618e-16 17 1.4630971223854737e-16
		 18 1.4031880544797365e-16 19 1.4883760465400598e-16 20 1.366196439980248e-16 21 1.4766773147717158e-16
		 22 1.6188582446743797e-16 23 1.4402029385734153e-16 24 1.397456363371502e-16 25 1.6218117382002537e-16
		 26 1.2150027677458715e-16 27 1.4816123362171339e-16 28 1.5323370237829871e-16 29 1.4828130704789166e-16
		 30 1.3300436754373932e-16 31 1.5374212860520541e-16 32 1.3668527537509101e-16 33 1.5567962994810635e-16
		 34 1.4659613610576086e-16 35 1.4094426657602504e-16 36 1.3673012586012084e-16 37 1.4542322440356262e-16
		 38 1.4463288599859167e-16 39 1.4435239158662081e-16 40 1.4398804892874575e-16;
createNode animCurveTA -n "torso_jnt_rotateY";
	rename -uid "00A720D0-4827-1F07-3A0F-549219369027";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 2.0411943907098658e-14 1 2.0360908732206163e-14
		 2 2.0430902764379143e-14 3 2.0248237200820823e-14 4 2.0168159788266244e-14 5 2.0293042421817759e-14
		 6 2.073419551796684e-14 7 1.9778152118893822e-14 8 2.0784062069085922e-14 9 2.1359374094944174e-14
		 10 2.0680139116923272e-14 11 2.0918220998817484e-14 12 2.0781403037395081e-14 13 2.0944176416728659e-14
		 14 2.0777031785276664e-14 15 2.0358656382185744e-14 16 2.0702176726334722e-14 17 1.9758231190856817e-14
		 18 1.8819603374865514e-14 19 2.0384634579522663e-14 20 1.8295205158614723e-14 21 1.9504002729532063e-14
		 22 1.651605601952661e-14 23 1.9701725973590296e-14 24 1.8380955623694218e-14 25 1.8702292207011925e-14
		 26 1.8653624427025777e-14 27 1.9573962272500367e-14 28 1.8863464383899894e-14 29 1.8161406008645421e-14
		 30 2.1714038199086201e-14 31 1.8935539585290396e-14 32 2.2235848757273906e-14 33 1.9210202441167289e-14
		 34 1.9352091895439099e-14 35 2.0337847642813199e-14 36 1.9204353493091568e-14 37 2.133018537584051e-14
		 38 2.0324659642509508e-14 39 2.0441256466703109e-14 40 2.0411943907098658e-14;
createNode animCurveTA -n "torso_jnt_rotateZ";
	rename -uid "55D642F3-4DDA-DD7C-217D-8585D64C70E8";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0.80832778160788832 1 0.80832778160698648
		 2 0.80832778160788832 3 0.80832778160698648 4 0.80832778160788832 5 0.80832778160698648
		 6 0.80832778160788832 7 0.80832778160788832 8 0.80832778160788832 9 0.80832778160788832
		 10 0.80832778160788832 11 0.80832778160788832 12 0.80832778160788843 13 0.80832778160698648
		 14 0.8083277816069866 15 0.80832778160788832 16 0.80832778160788832 17 0.80832778160698648
		 18 0.80832778160788832 19 0.80832778160698648 20 0.80832778160788832 21 0.80832778160788832
		 22 0.80832778160788832 23 0.80832778160698648 24 0.80832778160788832 25 0.80832778160698648
		 26 0.80832778160698648 27 0.80832778160698648 28 0.80832778160788832 29 0.80832778160698648
		 30 0.80832778160788832 31 0.80832778160788832 32 0.80832778160698648 33 0.80832778160788832
		 34 0.80832778160788832 35 0.80832778160698648 36 0.80832778160788832 37 0.80832778160698648
		 38 0.80832778160788832 39 0.80832778160698648 40 0.80832778160788832;
createNode animCurveTU -n "torso_jnt_scaleX";
	rename -uid "82074B41-428A-3F2F-DC6D-08BF26DF8C33";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "torso_jnt_scaleY";
	rename -uid "05E944F8-405B-C506-4BB7-9D859480E9E7";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "torso_jnt_scaleZ";
	rename -uid "EC7F5A0F-44C5-0BC8-4D3B-0184E2D42853";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "torso_splineik_jnt_visibility";
	rename -uid "E19EDB09-4F0B-BB74-11A4-D4B0F45D43AF";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "torso_splineik_jnt_translateX";
	rename -uid "A6987F3E-4180-E494-1B59-0FA832085A5B";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 3.1554436208840472e-30 1 1.3877787807814457e-17
		 2 -5.5511151231257827e-17 3 0 4 0 5 -2.2204460492503131e-16 6 6.6613381477509392e-16
		 7 -2.2204460492503131e-16 8 -4.4408920985006262e-16 9 6.6613381477509392e-16 10 0
		 11 2.2204460492503131e-16 12 0 13 3.3306690738754696e-16 14 -1.1102230246251565e-16
		 15 5.5511151231257827e-17 16 1.3877787807814457e-16 17 -1.1102230246251565e-16 18 -3.8857805861880479e-16
		 19 -8.0491169285323849e-16 20 -3.6082248300317588e-16 21 -8.8817841970012523e-16
		 22 2.2204460492503131e-16 23 0 24 -6.6613381477509392e-16 25 -4.4408920985006262e-16
		 26 -8.8817841970012523e-16 27 4.4408920985006262e-16 28 0 29 0 30 0 31 -1.7763568394002505e-15
		 32 -8.8817841970012523e-16 33 0 34 1.1102230246251565e-16 35 -2.2204460492503131e-16
		 36 -4.4408920985006262e-16 37 0 38 -1.6653345369377348e-16 39 2.0816681711721685e-17
		 40 3.1554436208840472e-30;
createNode animCurveTL -n "torso_splineik_jnt_translateY";
	rename -uid "DA5253D5-4CC2-BFA5-D1F6-A184F2F464B6";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 14.000664705468608 1 14.000664705468637
		 2 14.00066470546858 3 14.000664705468637 4 14.000664705468608 5 14.000664705468608
		 6 14.000664705468637 7 14.000664705468594 8 14.000664705468566 9 14.000664705468623
		 10 14.000664705468623 11 14.000664705468608 12 14.000664705468623 13 14.000664705468623
		 14 14.000664705468608 15 14.000664705468594 16 14.000664705468608 17 14.000664705468594
		 18 14.000664705468623 19 14.000664705468608 20 14.000664705468651 21 14.000664705468623
		 22 14.00066470546858 23 14.000664705468608 24 14.000664705468608 25 14.000664705468651
		 26 14.000664705468623 27 14.000664705468608 28 14.000664705468608 29 14.000664705468608
		 30 14.000664705468637 31 14.000664705468623 32 14.000664705468623 33 14.000664705468637
		 34 14.000664705468594 35 14.000664705468608 36 14.000664705468623 37 14.000664705468608
		 38 14.000664705468608 39 14.000664705468594 40 14.000664705468608;
createNode animCurveTL -n "torso_splineik_jnt_translateZ";
	rename -uid "33954ED2-4E90-802B-320E-47868A8DF349";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 16.516968660242227 1 16.516968660242235
		 2 16.516968660242227 3 16.516968660242231 4 16.516968660242231 5 16.516968660242231
		 6 16.516968660242227 7 16.516968660242227 8 16.516968660242235 9 16.516968660242235
		 10 16.516968660242227 11 16.516968660242227 12 16.516968660242231 13 16.516968660242231
		 14 16.516968660242235 15 16.516968660242231 16 16.516968660242227 17 16.516968660242224
		 18 16.516968660242231 19 16.516968660242235 20 16.516968660242227 21 16.516968660242227
		 22 16.516968660242227 23 16.516968660242227 24 16.516968660242235 25 16.516968660242231
		 26 16.516968660242227 27 16.51696866024222 28 16.516968660242227 29 16.516968660242227
		 30 16.516968660242231 31 16.516968660242227 32 16.516968660242227 33 16.516968660242235
		 34 16.516968660242227 35 16.516968660242231 36 16.516968660242235 37 16.516968660242231
		 38 16.516968660242227 39 16.516968660242227 40 16.516968660242227;
createNode animCurveTA -n "torso_splineik_jnt_rotateX";
	rename -uid "0EE0A3AB-4015-8D8F-D643-A8999315F9CE";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "torso_splineik_jnt_rotateY";
	rename -uid "4FEEED22-49B6-E19A-36B9-37B41AE3E23F";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "torso_splineik_jnt_rotateZ";
	rename -uid "0538FFEC-4838-1DCF-46DF-98A2D9ED4634";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "torso_splineik_jnt_scaleX";
	rename -uid "22B6B44C-411D-2D0C-682B-39AAC9165518";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "torso_splineik_jnt_scaleY";
	rename -uid "E5DB8B40-4CF0-9C7D-8B5F-8DA39E218D9C";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "torso_splineik_jnt_scaleZ";
	rename -uid "41F07294-418C-07CA-EAF6-27A41E5D8908";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "upperlip_jnt_visibility";
	rename -uid "2AE601B2-434B-7B43-2D0B-BFBD516ECE87";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTL -n "upperlip_jnt_translateX";
	rename -uid "76CB951A-4A97-15CA-22AC-45831E9C024A";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 41.795914335240418 1 41.795914702209103
		 2 41.795914335240411 3 41.795914335240425 4 41.795914335240397 5 41.795914335240425
		 6 41.795914335240433 7 41.79591433524044 8 41.795914335240411 9 41.795914335240411
		 10 41.795914335240397 11 41.795914335240425 12 41.795914335240418 13 41.795914335240433
		 14 41.795914335240397 15 41.795914335240454 16 41.795914335240433 17 41.79591460400114
		 18 41.795914335240411 19 41.795914335240418 20 41.795914335240433 21 41.795914335240418
		 22 41.79591433524039 23 41.795914335240425 24 41.795914335240433 25 41.79591433524044
		 26 41.795914335240397 27 41.795914335240433 28 41.79591433524044 29 41.79591433524044
		 30 41.795914335240418 31 41.795914335240404 32 41.79591433524044 33 41.79591433524044
		 34 41.795914335240411 35 41.795914335240397 36 41.79591433524039 37 41.79591433524044
		 38 41.795914335240425 39 41.795914335240425 40 41.795914335240433;
createNode animCurveTL -n "upperlip_jnt_translateY";
	rename -uid "31D7C783-4FD1-A3DE-2599-1199443C78F2";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 30.355167608943674 1 30.355167103665821
		 2 30.35516760894366 3 30.355167608943702 4 30.35516760894366 5 30.355167608943688
		 6 30.355167608943688 7 30.35516760894366 8 30.35516760894366 9 30.35516760894366
		 10 30.355167608943674 11 30.355167608943674 12 30.35516760894366 13 30.355167608943674
		 14 30.355167608943688 15 30.355167608943674 16 30.355167608943674 17 30.355167238888072
		 18 30.355167608943717 19 30.355167608943646 20 30.355167608943717 21 30.355167608943688
		 22 30.355167608943702 23 30.355167608943702 24 30.355167608943674 25 30.355167608943646
		 26 30.355167608943674 27 30.355167608943674 28 30.355167608943688 29 30.355167608943702
		 30 30.355167608943688 31 30.355167608943702 32 30.355167608943674 33 30.355167608943688
		 34 30.355167608943688 35 30.355167608943702 36 30.35516760894366 37 30.355167608943717
		 38 30.355167608943646 39 30.355167608943688 40 30.355167608943702;
createNode animCurveTL -n "upperlip_jnt_translateZ";
	rename -uid "AB7EBD84-4911-8699-D1ED-F1B936EAAA73";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 -1.6098319189801934e-15 1 2.9176605575997883e-11
		 2 -2.1260770921571748e-14 3 -2.1427304375265521e-14 4 -2.0650148258027912e-14 5 -2.042810365310288e-14
		 6 -2.2204460492503131e-14 7 -1.4210854715202004e-14 8 -1.7763568394002505e-14 9 -2.8421709430404007e-14
		 10 -1.9539925233402755e-14 11 -2.1316282072803006e-14 12 -3.5527136788005009e-14
		 13 -2.6645352591003757e-14 14 -1.7763568394002505e-14 15 -2.1316282072803006e-14
		 16 -2.2204460492503131e-14 17 -1.8320733818910639e-11 18 -2.042810365310288e-14 19 -1.9539925233402755e-14
		 20 -1.7763568394002505e-14 21 -2.3980817331903381e-14 22 -2.1316282072803006e-14
		 23 -1.6875389974302379e-14 24 -2.3980817331903381e-14 25 -1.7763568394002505e-14
		 26 -2.3092638912203256e-14 27 -2.2204460492503131e-14 28 -1.9539925233402755e-14
		 29 -2.0872192862952943e-14 30 -1.9984014443252818e-14 31 -2.1760371282653068e-14
		 32 -2.1760371282653068e-14 33 -2.1649348980190553e-14 34 -2.1760371282653068e-14
		 35 -2.0650148258027912e-14 36 -2.0872192862952943e-14 37 -2.1094237467877974e-14
		 38 -2.1316282072803006e-14 39 -2.1482815526496779e-14 40 -1.6253703493778169e-15;
createNode animCurveTA -n "upperlip_jnt_rotateX";
	rename -uid "A8529F02-4E6A-F68B-04CF-6291810A50BF";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "upperlip_jnt_rotateY";
	rename -uid "53DD1A73-4F4B-A77D-7AC4-D8BD7E5A4641";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTA -n "upperlip_jnt_rotateZ";
	rename -uid "1620305B-49A5-5C1C-D19A-F888DAB231C4";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0
		 10 0 11 0 12 0 13 0 14 0 15 0 16 0 17 0 18 0 19 0 20 0 21 0 22 0 23 0 24 0 25 0 26 0
		 27 0 28 0 29 0 30 0 31 0 32 0 33 0 34 0 35 0 36 0 37 0 38 0 39 0 40 0;
createNode animCurveTU -n "upperlip_jnt_scaleX";
	rename -uid "E8268962-4422-FBFF-66B5-01A2760BC538";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "upperlip_jnt_scaleY";
	rename -uid "ACFD8696-4FEF-70B3-0DBB-B4805171BAB2";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
createNode animCurveTU -n "upperlip_jnt_scaleZ";
	rename -uid "795779D4-44B8-7E01-1429-F2B3376012B0";
	setAttr ".tan" 18;
	setAttr ".wgt" no;
	setAttr -s 41 ".ktv[0:40]"  0 1 1 1 2 1 3 1 4 1 5 1 6 1 7 1 8 1 9 1
		 10 1 11 1 12 1 13 1 14 1 15 1 16 1 17 1 18 1 19 1 20 1 21 1 22 1 23 1 24 1 25 1 26 1
		 27 1 28 1 29 1 30 1 31 1 32 1 33 1 34 1 35 1 36 1 37 1 38 1 39 1 40 1;
select -ne :time1;
	setAttr ".o" 15;
	setAttr ".unw" 15;
select -ne :hardwareRenderingGlobals;
	setAttr ".otfna" -type "stringArray" 22 "NURBS Curves" "NURBS Surfaces" "Polygons" "Subdiv Surface" "Particles" "Particle Instance" "Fluids" "Strokes" "Image Planes" "UI" "Lights" "Cameras" "Locators" "Joints" "IK Handles" "Deformers" "Motion Trails" "Components" "Hair Systems" "Follicles" "Misc. UI" "Ornaments"  ;
	setAttr ".otfva" -type "Int32Array" 22 0 1 1 1 1 1
		 1 1 1 0 0 0 0 0 0 0 0 0
		 0 0 0 0 ;
	setAttr ".fprt" yes;
select -ne :renderPartition;
	setAttr -s 14 ".st";
select -ne :renderGlobalsList1;
select -ne :defaultShaderList1;
	setAttr -s 14 ".s";
select -ne :postProcessList1;
	setAttr -s 2 ".p";
select -ne :defaultRenderUtilityList1;
	setAttr -s 3 ".u";
select -ne :defaultRenderingList1;
select -ne :defaultTextureList1;
	setAttr -s 3 ".tx";
select -ne :initialShadingGroup;
	setAttr ".ro" yes;
select -ne :initialParticleSE;
	setAttr ".ro" yes;
select -ne :defaultRenderGlobals;
	setAttr ".ren" -type "string" "arnold";
select -ne :defaultResolution;
	setAttr ".pa" 1;
select -ne :hardwareRenderGlobals;
	setAttr ".ctrs" 256;
	setAttr ".btrs" 512;
select -ne :ikSystem;
	setAttr -s 4 ".sol";
connectAttr "joints.di" "joints_grp.do";
connectAttr "BallMonster_root_jnt_translateX.o" "BallMonster_root_jnt.tx";
connectAttr "BallMonster_root_jnt_translateY.o" "BallMonster_root_jnt.ty";
connectAttr "BallMonster_root_jnt_translateZ.o" "BallMonster_root_jnt.tz";
connectAttr "BallMonster_root_jnt_rotateX.o" "BallMonster_root_jnt.rx";
connectAttr "BallMonster_root_jnt_rotateY.o" "BallMonster_root_jnt.ry";
connectAttr "BallMonster_root_jnt_rotateZ.o" "BallMonster_root_jnt.rz";
connectAttr "BallMonster_root_jnt_scaleX.o" "BallMonster_root_jnt.sx";
connectAttr "BallMonster_root_jnt_scaleY.o" "BallMonster_root_jnt.sy";
connectAttr "BallMonster_root_jnt_scaleZ.o" "BallMonster_root_jnt.sz";
connectAttr "BallMonster_root_jnt_visibility.o" "BallMonster_root_jnt.v";
connectAttr "BallMonster_root_jnt.s" "COG_jnt.is";
connectAttr "COG_jnt_translateX.o" "COG_jnt.tx";
connectAttr "COG_jnt_translateY.o" "COG_jnt.ty";
connectAttr "COG_jnt_translateZ.o" "COG_jnt.tz";
connectAttr "COG_jnt_rotateX.o" "COG_jnt.rx";
connectAttr "COG_jnt_rotateY.o" "COG_jnt.ry";
connectAttr "COG_jnt_rotateZ.o" "COG_jnt.rz";
connectAttr "COG_jnt_scaleX.o" "COG_jnt.sx";
connectAttr "COG_jnt_scaleY.o" "COG_jnt.sy";
connectAttr "COG_jnt_scaleZ.o" "COG_jnt.sz";
connectAttr "COG_jnt_visibility.o" "COG_jnt.v";
connectAttr "COG_jnt.s" "torso_jnt.is";
connectAttr "torso_jnt_scaleX.o" "torso_jnt.sx";
connectAttr "torso_jnt_scaleY.o" "torso_jnt.sy";
connectAttr "torso_jnt_scaleZ.o" "torso_jnt.sz";
connectAttr "torso_jnt_rotateX.o" "torso_jnt.rx";
connectAttr "torso_jnt_rotateY.o" "torso_jnt.ry";
connectAttr "torso_jnt_rotateZ.o" "torso_jnt.rz";
connectAttr "torso_jnt_translateX.o" "torso_jnt.tx";
connectAttr "torso_jnt_translateY.o" "torso_jnt.ty";
connectAttr "torso_jnt_translateZ.o" "torso_jnt.tz";
connectAttr "torso_jnt_visibility.o" "torso_jnt.v";
connectAttr "torso_jnt.s" "spine2_jnt.is";
connectAttr "spine2_jnt_scaleX.o" "spine2_jnt.sx";
connectAttr "spine2_jnt_scaleY.o" "spine2_jnt.sy";
connectAttr "spine2_jnt_scaleZ.o" "spine2_jnt.sz";
connectAttr "spine2_jnt_rotateX.o" "spine2_jnt.rx";
connectAttr "spine2_jnt_rotateY.o" "spine2_jnt.ry";
connectAttr "spine2_jnt_rotateZ.o" "spine2_jnt.rz";
connectAttr "spine2_jnt_visibility.o" "spine2_jnt.v";
connectAttr "spine2_jnt_translateX.o" "spine2_jnt.tx";
connectAttr "spine2_jnt_translateY.o" "spine2_jnt.ty";
connectAttr "spine2_jnt_translateZ.o" "spine2_jnt.tz";
connectAttr "spine2_jnt.s" "spine1_jnt.is";
connectAttr "spine1_jnt_scaleX.o" "spine1_jnt.sx";
connectAttr "spine1_jnt_scaleY.o" "spine1_jnt.sy";
connectAttr "spine1_jnt_scaleZ.o" "spine1_jnt.sz";
connectAttr "spine1_jnt_rotateX.o" "spine1_jnt.rx";
connectAttr "spine1_jnt_rotateY.o" "spine1_jnt.ry";
connectAttr "spine1_jnt_rotateZ.o" "spine1_jnt.rz";
connectAttr "spine1_jnt_visibility.o" "spine1_jnt.v";
connectAttr "spine1_jnt_translateX.o" "spine1_jnt.tx";
connectAttr "spine1_jnt_translateY.o" "spine1_jnt.ty";
connectAttr "spine1_jnt_translateZ.o" "spine1_jnt.tz";
connectAttr "spine1_jnt.s" "pelvis_jnt.is";
connectAttr "pelvis_jnt_scaleX.o" "pelvis_jnt.sx";
connectAttr "pelvis_jnt_scaleY.o" "pelvis_jnt.sy";
connectAttr "pelvis_jnt_scaleZ.o" "pelvis_jnt.sz";
connectAttr "pelvis_jnt_translateX.o" "pelvis_jnt.tx";
connectAttr "pelvis_jnt_translateY.o" "pelvis_jnt.ty";
connectAttr "pelvis_jnt_translateZ.o" "pelvis_jnt.tz";
connectAttr "pelvis_jnt_visibility.o" "pelvis_jnt.v";
connectAttr "pelvis_jnt_rotateX.o" "pelvis_jnt.rx";
connectAttr "pelvis_jnt_rotateY.o" "pelvis_jnt.ry";
connectAttr "pelvis_jnt_rotateZ.o" "pelvis_jnt.rz";
connectAttr "pelvis_jnt.s" "tailbase_jnt.is";
connectAttr "tailbase_jnt_translateX.o" "tailbase_jnt.tx";
connectAttr "tailbase_jnt_translateY.o" "tailbase_jnt.ty";
connectAttr "tailbase_jnt_translateZ.o" "tailbase_jnt.tz";
connectAttr "tailbase_jnt_rotateX.o" "tailbase_jnt.rx";
connectAttr "tailbase_jnt_rotateY.o" "tailbase_jnt.ry";
connectAttr "tailbase_jnt_rotateZ.o" "tailbase_jnt.rz";
connectAttr "tailbase_jnt_scaleX.o" "tailbase_jnt.sx";
connectAttr "tailbase_jnt_scaleY.o" "tailbase_jnt.sy";
connectAttr "tailbase_jnt_scaleZ.o" "tailbase_jnt.sz";
connectAttr "tailbase_jnt_visibility.o" "tailbase_jnt.v";
connectAttr "tailbase_jnt.s" "tail_jnt.is";
connectAttr "tail_jnt_visibility.o" "tail_jnt.v";
connectAttr "tail_jnt_translateX.o" "tail_jnt.tx";
connectAttr "tail_jnt_translateY.o" "tail_jnt.ty";
connectAttr "tail_jnt_translateZ.o" "tail_jnt.tz";
connectAttr "tail_jnt_rotateX.o" "tail_jnt.rx";
connectAttr "tail_jnt_rotateY.o" "tail_jnt.ry";
connectAttr "tail_jnt_rotateZ.o" "tail_jnt.rz";
connectAttr "tail_jnt_scaleX.o" "tail_jnt.sx";
connectAttr "tail_jnt_scaleY.o" "tail_jnt.sy";
connectAttr "tail_jnt_scaleZ.o" "tail_jnt.sz";
connectAttr "pelvis_jnt.s" "L_B_hip_jnt.is";
connectAttr "L_B_hip_jnt_scaleX.o" "L_B_hip_jnt.sx";
connectAttr "L_B_hip_jnt_scaleY.o" "L_B_hip_jnt.sy";
connectAttr "L_B_hip_jnt_scaleZ.o" "L_B_hip_jnt.sz";
connectAttr "L_B_hip_jnt_translateX.o" "L_B_hip_jnt.tx";
connectAttr "L_B_hip_jnt_translateY.o" "L_B_hip_jnt.ty";
connectAttr "L_B_hip_jnt_translateZ.o" "L_B_hip_jnt.tz";
connectAttr "L_B_hip_jnt_rotateX.o" "L_B_hip_jnt.rx";
connectAttr "L_B_hip_jnt_rotateY.o" "L_B_hip_jnt.ry";
connectAttr "L_B_hip_jnt_rotateZ.o" "L_B_hip_jnt.rz";
connectAttr "L_B_hip_jnt_visibility.o" "L_B_hip_jnt.v";
connectAttr "L_B_hip_jnt.s" "L_B_knee_jnt.is";
connectAttr "L_B_knee_jnt_scaleX.o" "L_B_knee_jnt.sx";
connectAttr "L_B_knee_jnt_scaleY.o" "L_B_knee_jnt.sy";
connectAttr "L_B_knee_jnt_scaleZ.o" "L_B_knee_jnt.sz";
connectAttr "L_B_knee_jnt_rotateX.o" "L_B_knee_jnt.rx";
connectAttr "L_B_knee_jnt_rotateY.o" "L_B_knee_jnt.ry";
connectAttr "L_B_knee_jnt_rotateZ.o" "L_B_knee_jnt.rz";
connectAttr "L_B_knee_jnt_visibility.o" "L_B_knee_jnt.v";
connectAttr "L_B_knee_jnt_translateX.o" "L_B_knee_jnt.tx";
connectAttr "L_B_knee_jnt_translateY.o" "L_B_knee_jnt.ty";
connectAttr "L_B_knee_jnt_translateZ.o" "L_B_knee_jnt.tz";
connectAttr "L_B_knee_jnt.s" "L_B_ankle_jnt.is";
connectAttr "L_B_ankle_jnt_scaleX.o" "L_B_ankle_jnt.sx";
connectAttr "L_B_ankle_jnt_scaleY.o" "L_B_ankle_jnt.sy";
connectAttr "L_B_ankle_jnt_scaleZ.o" "L_B_ankle_jnt.sz";
connectAttr "L_B_ankle_jnt_translateX.o" "L_B_ankle_jnt.tx";
connectAttr "L_B_ankle_jnt_translateY.o" "L_B_ankle_jnt.ty";
connectAttr "L_B_ankle_jnt_translateZ.o" "L_B_ankle_jnt.tz";
connectAttr "L_B_ankle_jnt_rotateX.o" "L_B_ankle_jnt.rx";
connectAttr "L_B_ankle_jnt_rotateY.o" "L_B_ankle_jnt.ry";
connectAttr "L_B_ankle_jnt_rotateZ.o" "L_B_ankle_jnt.rz";
connectAttr "L_B_ankle_jnt_visibility.o" "L_B_ankle_jnt.v";
connectAttr "L_B_ankle_jnt.s" "L_B_toe_jnt.is";
connectAttr "L_B_toe_jnt_translateX.o" "L_B_toe_jnt.tx";
connectAttr "L_B_toe_jnt_translateY.o" "L_B_toe_jnt.ty";
connectAttr "L_B_toe_jnt_translateZ.o" "L_B_toe_jnt.tz";
connectAttr "L_B_toe_jnt_visibility.o" "L_B_toe_jnt.v";
connectAttr "L_B_toe_jnt_rotateX.o" "L_B_toe_jnt.rx";
connectAttr "L_B_toe_jnt_rotateY.o" "L_B_toe_jnt.ry";
connectAttr "L_B_toe_jnt_rotateZ.o" "L_B_toe_jnt.rz";
connectAttr "L_B_toe_jnt_scaleX.o" "L_B_toe_jnt.sx";
connectAttr "L_B_toe_jnt_scaleY.o" "L_B_toe_jnt.sy";
connectAttr "L_B_toe_jnt_scaleZ.o" "L_B_toe_jnt.sz";
connectAttr "pelvis_jnt.s" "R_B_hip_jnt.is";
connectAttr "R_B_hip_jnt_scaleX.o" "R_B_hip_jnt.sx";
connectAttr "R_B_hip_jnt_scaleY.o" "R_B_hip_jnt.sy";
connectAttr "R_B_hip_jnt_scaleZ.o" "R_B_hip_jnt.sz";
connectAttr "R_B_hip_jnt_translateX.o" "R_B_hip_jnt.tx";
connectAttr "R_B_hip_jnt_translateY.o" "R_B_hip_jnt.ty";
connectAttr "R_B_hip_jnt_translateZ.o" "R_B_hip_jnt.tz";
connectAttr "R_B_hip_jnt_rotateX.o" "R_B_hip_jnt.rx";
connectAttr "R_B_hip_jnt_rotateY.o" "R_B_hip_jnt.ry";
connectAttr "R_B_hip_jnt_rotateZ.o" "R_B_hip_jnt.rz";
connectAttr "R_B_hip_jnt_visibility.o" "R_B_hip_jnt.v";
connectAttr "R_B_hip_jnt.s" "R_B_knee_jnt.is";
connectAttr "R_B_knee_jnt_scaleX.o" "R_B_knee_jnt.sx";
connectAttr "R_B_knee_jnt_scaleY.o" "R_B_knee_jnt.sy";
connectAttr "R_B_knee_jnt_scaleZ.o" "R_B_knee_jnt.sz";
connectAttr "R_B_knee_jnt_rotateX.o" "R_B_knee_jnt.rx";
connectAttr "R_B_knee_jnt_rotateY.o" "R_B_knee_jnt.ry";
connectAttr "R_B_knee_jnt_rotateZ.o" "R_B_knee_jnt.rz";
connectAttr "R_B_knee_jnt_visibility.o" "R_B_knee_jnt.v";
connectAttr "R_B_knee_jnt_translateX.o" "R_B_knee_jnt.tx";
connectAttr "R_B_knee_jnt_translateY.o" "R_B_knee_jnt.ty";
connectAttr "R_B_knee_jnt_translateZ.o" "R_B_knee_jnt.tz";
connectAttr "R_B_knee_jnt.s" "R_B_ankle_jnt.is";
connectAttr "R_B_ankle_jnt_scaleX.o" "R_B_ankle_jnt.sx";
connectAttr "R_B_ankle_jnt_scaleY.o" "R_B_ankle_jnt.sy";
connectAttr "R_B_ankle_jnt_scaleZ.o" "R_B_ankle_jnt.sz";
connectAttr "R_B_ankle_jnt_translateX.o" "R_B_ankle_jnt.tx";
connectAttr "R_B_ankle_jnt_translateY.o" "R_B_ankle_jnt.ty";
connectAttr "R_B_ankle_jnt_translateZ.o" "R_B_ankle_jnt.tz";
connectAttr "R_B_ankle_jnt_rotateX.o" "R_B_ankle_jnt.rx";
connectAttr "R_B_ankle_jnt_rotateY.o" "R_B_ankle_jnt.ry";
connectAttr "R_B_ankle_jnt_rotateZ.o" "R_B_ankle_jnt.rz";
connectAttr "R_B_ankle_jnt_visibility.o" "R_B_ankle_jnt.v";
connectAttr "R_B_ankle_jnt.s" "R_B_toe_jnt.is";
connectAttr "R_B_toe_jnt_translateX.o" "R_B_toe_jnt.tx";
connectAttr "R_B_toe_jnt_translateY.o" "R_B_toe_jnt.ty";
connectAttr "R_B_toe_jnt_translateZ.o" "R_B_toe_jnt.tz";
connectAttr "R_B_toe_jnt_visibility.o" "R_B_toe_jnt.v";
connectAttr "R_B_toe_jnt_rotateX.o" "R_B_toe_jnt.rx";
connectAttr "R_B_toe_jnt_rotateY.o" "R_B_toe_jnt.ry";
connectAttr "R_B_toe_jnt_rotateZ.o" "R_B_toe_jnt.rz";
connectAttr "R_B_toe_jnt_scaleX.o" "R_B_toe_jnt.sx";
connectAttr "R_B_toe_jnt_scaleY.o" "R_B_toe_jnt.sy";
connectAttr "R_B_toe_jnt_scaleZ.o" "R_B_toe_jnt.sz";
connectAttr "torso_jnt.s" "head_jnt.is";
connectAttr "head_jnt_translateX.o" "head_jnt.tx";
connectAttr "head_jnt_translateY.o" "head_jnt.ty";
connectAttr "head_jnt_translateZ.o" "head_jnt.tz";
connectAttr "head_jnt_rotateX.o" "head_jnt.rx";
connectAttr "head_jnt_rotateY.o" "head_jnt.ry";
connectAttr "head_jnt_rotateZ.o" "head_jnt.rz";
connectAttr "head_jnt_scaleX.o" "head_jnt.sx";
connectAttr "head_jnt_scaleY.o" "head_jnt.sy";
connectAttr "head_jnt_scaleZ.o" "head_jnt.sz";
connectAttr "head_jnt_visibility.o" "head_jnt.v";
connectAttr "head_jnt.s" "jaw_jnt.is";
connectAttr "jaw_jnt_translateX.o" "jaw_jnt.tx";
connectAttr "jaw_jnt_translateY.o" "jaw_jnt.ty";
connectAttr "jaw_jnt_translateZ.o" "jaw_jnt.tz";
connectAttr "jaw_jnt_rotateX.o" "jaw_jnt.rx";
connectAttr "jaw_jnt_rotateY.o" "jaw_jnt.ry";
connectAttr "jaw_jnt_rotateZ.o" "jaw_jnt.rz";
connectAttr "jaw_jnt_scaleX.o" "jaw_jnt.sx";
connectAttr "jaw_jnt_scaleY.o" "jaw_jnt.sy";
connectAttr "jaw_jnt_scaleZ.o" "jaw_jnt.sz";
connectAttr "jaw_jnt_visibility.o" "jaw_jnt.v";
connectAttr "jaw_jnt.s" "tonguebase_jnt.is";
connectAttr "tonguebase_jnt_translateX.o" "tonguebase_jnt.tx";
connectAttr "tonguebase_jnt_translateY.o" "tonguebase_jnt.ty";
connectAttr "tonguebase_jnt_translateZ.o" "tonguebase_jnt.tz";
connectAttr "tonguebase_jnt_rotateX.o" "tonguebase_jnt.rx";
connectAttr "tonguebase_jnt_rotateY.o" "tonguebase_jnt.ry";
connectAttr "tonguebase_jnt_rotateZ.o" "tonguebase_jnt.rz";
connectAttr "tonguebase_jnt_scaleX.o" "tonguebase_jnt.sx";
connectAttr "tonguebase_jnt_scaleY.o" "tonguebase_jnt.sy";
connectAttr "tonguebase_jnt_scaleZ.o" "tonguebase_jnt.sz";
connectAttr "tonguebase_jnt_visibility.o" "tonguebase_jnt.v";
connectAttr "tonguebase_jnt.s" "tonguemid_jnt.is";
connectAttr "tonguemid_jnt_translateX.o" "tonguemid_jnt.tx";
connectAttr "tonguemid_jnt_translateY.o" "tonguemid_jnt.ty";
connectAttr "tonguemid_jnt_translateZ.o" "tonguemid_jnt.tz";
connectAttr "tonguemid_jnt_rotateX.o" "tonguemid_jnt.rx";
connectAttr "tonguemid_jnt_rotateY.o" "tonguemid_jnt.ry";
connectAttr "tonguemid_jnt_rotateZ.o" "tonguemid_jnt.rz";
connectAttr "tonguemid_jnt_scaleX.o" "tonguemid_jnt.sx";
connectAttr "tonguemid_jnt_scaleY.o" "tonguemid_jnt.sy";
connectAttr "tonguemid_jnt_scaleZ.o" "tonguemid_jnt.sz";
connectAttr "tonguemid_jnt_visibility.o" "tonguemid_jnt.v";
connectAttr "tonguemid_jnt.s" "tonguetip_jnt.is";
connectAttr "tonguetip_jnt_translateX.o" "tonguetip_jnt.tx";
connectAttr "tonguetip_jnt_translateY.o" "tonguetip_jnt.ty";
connectAttr "tonguetip_jnt_translateZ.o" "tonguetip_jnt.tz";
connectAttr "tonguetip_jnt_rotateX.o" "tonguetip_jnt.rx";
connectAttr "tonguetip_jnt_rotateY.o" "tonguetip_jnt.ry";
connectAttr "tonguetip_jnt_rotateZ.o" "tonguetip_jnt.rz";
connectAttr "tonguetip_jnt_scaleX.o" "tonguetip_jnt.sx";
connectAttr "tonguetip_jnt_scaleY.o" "tonguetip_jnt.sy";
connectAttr "tonguetip_jnt_scaleZ.o" "tonguetip_jnt.sz";
connectAttr "tonguetip_jnt_visibility.o" "tonguetip_jnt.v";
connectAttr "jaw_jnt.s" "L_lowerlip2_jnt.is";
connectAttr "L_lowerlip2_jnt_translateX.o" "L_lowerlip2_jnt.tx";
connectAttr "L_lowerlip2_jnt_translateY.o" "L_lowerlip2_jnt.ty";
connectAttr "L_lowerlip2_jnt_translateZ.o" "L_lowerlip2_jnt.tz";
connectAttr "L_lowerlip2_jnt_rotateX.o" "L_lowerlip2_jnt.rx";
connectAttr "L_lowerlip2_jnt_rotateY.o" "L_lowerlip2_jnt.ry";
connectAttr "L_lowerlip2_jnt_rotateZ.o" "L_lowerlip2_jnt.rz";
connectAttr "L_lowerlip2_jnt_visibility.o" "L_lowerlip2_jnt.v";
connectAttr "L_lowerlip2_jnt_scaleX.o" "L_lowerlip2_jnt.sx";
connectAttr "L_lowerlip2_jnt_scaleY.o" "L_lowerlip2_jnt.sy";
connectAttr "L_lowerlip2_jnt_scaleZ.o" "L_lowerlip2_jnt.sz";
connectAttr "jaw_jnt.s" "L_lowerlip1_jnt.is";
connectAttr "L_lowerlip1_jnt_translateX.o" "L_lowerlip1_jnt.tx";
connectAttr "L_lowerlip1_jnt_translateY.o" "L_lowerlip1_jnt.ty";
connectAttr "L_lowerlip1_jnt_translateZ.o" "L_lowerlip1_jnt.tz";
connectAttr "L_lowerlip1_jnt_visibility.o" "L_lowerlip1_jnt.v";
connectAttr "L_lowerlip1_jnt_rotateX.o" "L_lowerlip1_jnt.rx";
connectAttr "L_lowerlip1_jnt_rotateY.o" "L_lowerlip1_jnt.ry";
connectAttr "L_lowerlip1_jnt_rotateZ.o" "L_lowerlip1_jnt.rz";
connectAttr "L_lowerlip1_jnt_scaleX.o" "L_lowerlip1_jnt.sx";
connectAttr "L_lowerlip1_jnt_scaleY.o" "L_lowerlip1_jnt.sy";
connectAttr "L_lowerlip1_jnt_scaleZ.o" "L_lowerlip1_jnt.sz";
connectAttr "jaw_jnt.s" "lowerlip.is";
connectAttr "lowerlip_translateX.o" "lowerlip.tx";
connectAttr "lowerlip_translateY.o" "lowerlip.ty";
connectAttr "lowerlip_translateZ.o" "lowerlip.tz";
connectAttr "lowerlip_rotateX.o" "lowerlip.rx";
connectAttr "lowerlip_rotateY.o" "lowerlip.ry";
connectAttr "lowerlip_rotateZ.o" "lowerlip.rz";
connectAttr "lowerlip_visibility.o" "lowerlip.v";
connectAttr "lowerlip_scaleX.o" "lowerlip.sx";
connectAttr "lowerlip_scaleY.o" "lowerlip.sy";
connectAttr "lowerlip_scaleZ.o" "lowerlip.sz";
connectAttr "jaw_jnt.s" "R_lowerlip2_jnt.is";
connectAttr "R_lowerlip2_jnt_translateX.o" "R_lowerlip2_jnt.tx";
connectAttr "R_lowerlip2_jnt_translateY.o" "R_lowerlip2_jnt.ty";
connectAttr "R_lowerlip2_jnt_translateZ.o" "R_lowerlip2_jnt.tz";
connectAttr "R_lowerlip2_jnt_rotateX.o" "R_lowerlip2_jnt.rx";
connectAttr "R_lowerlip2_jnt_rotateY.o" "R_lowerlip2_jnt.ry";
connectAttr "R_lowerlip2_jnt_rotateZ.o" "R_lowerlip2_jnt.rz";
connectAttr "R_lowerlip2_jnt_visibility.o" "R_lowerlip2_jnt.v";
connectAttr "R_lowerlip2_jnt_scaleX.o" "R_lowerlip2_jnt.sx";
connectAttr "R_lowerlip2_jnt_scaleY.o" "R_lowerlip2_jnt.sy";
connectAttr "R_lowerlip2_jnt_scaleZ.o" "R_lowerlip2_jnt.sz";
connectAttr "jaw_jnt.s" "R_lowerlip1_jnt.is";
connectAttr "R_lowerlip1_jnt_translateX.o" "R_lowerlip1_jnt.tx";
connectAttr "R_lowerlip1_jnt_translateY.o" "R_lowerlip1_jnt.ty";
connectAttr "R_lowerlip1_jnt_translateZ.o" "R_lowerlip1_jnt.tz";
connectAttr "R_lowerlip1_jnt_rotateX.o" "R_lowerlip1_jnt.rx";
connectAttr "R_lowerlip1_jnt_rotateY.o" "R_lowerlip1_jnt.ry";
connectAttr "R_lowerlip1_jnt_rotateZ.o" "R_lowerlip1_jnt.rz";
connectAttr "R_lowerlip1_jnt_visibility.o" "R_lowerlip1_jnt.v";
connectAttr "R_lowerlip1_jnt_scaleX.o" "R_lowerlip1_jnt.sx";
connectAttr "R_lowerlip1_jnt_scaleY.o" "R_lowerlip1_jnt.sy";
connectAttr "R_lowerlip1_jnt_scaleZ.o" "R_lowerlip1_jnt.sz";
connectAttr "head_jnt.s" "L_upperlip1_jnt.is";
connectAttr "L_upperlip1_jnt_translateX.o" "L_upperlip1_jnt.tx";
connectAttr "L_upperlip1_jnt_translateY.o" "L_upperlip1_jnt.ty";
connectAttr "L_upperlip1_jnt_translateZ.o" "L_upperlip1_jnt.tz";
connectAttr "L_upperlip1_jnt_rotateX.o" "L_upperlip1_jnt.rx";
connectAttr "L_upperlip1_jnt_rotateY.o" "L_upperlip1_jnt.ry";
connectAttr "L_upperlip1_jnt_rotateZ.o" "L_upperlip1_jnt.rz";
connectAttr "L_upperlip1_jnt_visibility.o" "L_upperlip1_jnt.v";
connectAttr "L_upperlip1_jnt_scaleX.o" "L_upperlip1_jnt.sx";
connectAttr "L_upperlip1_jnt_scaleY.o" "L_upperlip1_jnt.sy";
connectAttr "L_upperlip1_jnt_scaleZ.o" "L_upperlip1_jnt.sz";
connectAttr "head_jnt.s" "L_upperlip2_jnt.is";
connectAttr "L_upperlip2_jnt_translateX.o" "L_upperlip2_jnt.tx";
connectAttr "L_upperlip2_jnt_translateY.o" "L_upperlip2_jnt.ty";
connectAttr "L_upperlip2_jnt_translateZ.o" "L_upperlip2_jnt.tz";
connectAttr "L_upperlip2_jnt_rotateX.o" "L_upperlip2_jnt.rx";
connectAttr "L_upperlip2_jnt_rotateY.o" "L_upperlip2_jnt.ry";
connectAttr "L_upperlip2_jnt_rotateZ.o" "L_upperlip2_jnt.rz";
connectAttr "L_upperlip2_jnt_visibility.o" "L_upperlip2_jnt.v";
connectAttr "L_upperlip2_jnt_scaleX.o" "L_upperlip2_jnt.sx";
connectAttr "L_upperlip2_jnt_scaleY.o" "L_upperlip2_jnt.sy";
connectAttr "L_upperlip2_jnt_scaleZ.o" "L_upperlip2_jnt.sz";
connectAttr "head_jnt.s" "L_lipcorner_jnt.is";
connectAttr "L_lipcorner_jnt_translateX.o" "L_lipcorner_jnt.tx";
connectAttr "L_lipcorner_jnt_translateY.o" "L_lipcorner_jnt.ty";
connectAttr "L_lipcorner_jnt_translateZ.o" "L_lipcorner_jnt.tz";
connectAttr "L_lipcorner_jnt_rotateX.o" "L_lipcorner_jnt.rx";
connectAttr "L_lipcorner_jnt_rotateY.o" "L_lipcorner_jnt.ry";
connectAttr "L_lipcorner_jnt_rotateZ.o" "L_lipcorner_jnt.rz";
connectAttr "L_lipcorner_jnt_visibility.o" "L_lipcorner_jnt.v";
connectAttr "L_lipcorner_jnt_scaleX.o" "L_lipcorner_jnt.sx";
connectAttr "L_lipcorner_jnt_scaleY.o" "L_lipcorner_jnt.sy";
connectAttr "L_lipcorner_jnt_scaleZ.o" "L_lipcorner_jnt.sz";
connectAttr "head_jnt.s" "L_cheek_jnt.is";
connectAttr "L_cheek_jnt_translateX.o" "L_cheek_jnt.tx";
connectAttr "L_cheek_jnt_translateY.o" "L_cheek_jnt.ty";
connectAttr "L_cheek_jnt_translateZ.o" "L_cheek_jnt.tz";
connectAttr "L_cheek_jnt_rotateX.o" "L_cheek_jnt.rx";
connectAttr "L_cheek_jnt_rotateY.o" "L_cheek_jnt.ry";
connectAttr "L_cheek_jnt_rotateZ.o" "L_cheek_jnt.rz";
connectAttr "L_cheek_jnt_scaleX.o" "L_cheek_jnt.sx";
connectAttr "L_cheek_jnt_scaleY.o" "L_cheek_jnt.sy";
connectAttr "L_cheek_jnt_scaleZ.o" "L_cheek_jnt.sz";
connectAttr "L_cheek_jnt_visibility.o" "L_cheek_jnt.v";
connectAttr "head_jnt.s" "L_brow_jnt.is";
connectAttr "L_brow_jnt_translateX.o" "L_brow_jnt.tx";
connectAttr "L_brow_jnt_translateY.o" "L_brow_jnt.ty";
connectAttr "L_brow_jnt_translateZ.o" "L_brow_jnt.tz";
connectAttr "L_brow_jnt_rotateX.o" "L_brow_jnt.rx";
connectAttr "L_brow_jnt_rotateY.o" "L_brow_jnt.ry";
connectAttr "L_brow_jnt_rotateZ.o" "L_brow_jnt.rz";
connectAttr "L_brow_jnt_visibility.o" "L_brow_jnt.v";
connectAttr "L_brow_jnt_scaleX.o" "L_brow_jnt.sx";
connectAttr "L_brow_jnt_scaleY.o" "L_brow_jnt.sy";
connectAttr "L_brow_jnt_scaleZ.o" "L_brow_jnt.sz";
connectAttr "head_jnt.s" "browmid_jnt.is";
connectAttr "browmid_jnt_translateX.o" "browmid_jnt.tx";
connectAttr "browmid_jnt_translateY.o" "browmid_jnt.ty";
connectAttr "browmid_jnt_translateZ.o" "browmid_jnt.tz";
connectAttr "browmid_jnt_rotateX.o" "browmid_jnt.rx";
connectAttr "browmid_jnt_rotateY.o" "browmid_jnt.ry";
connectAttr "browmid_jnt_rotateZ.o" "browmid_jnt.rz";
connectAttr "browmid_jnt_visibility.o" "browmid_jnt.v";
connectAttr "browmid_jnt_scaleX.o" "browmid_jnt.sx";
connectAttr "browmid_jnt_scaleY.o" "browmid_jnt.sy";
connectAttr "browmid_jnt_scaleZ.o" "browmid_jnt.sz";
connectAttr "head_jnt.s" "fluffbase_jnt.is";
connectAttr "fluffbase_jnt_translateX.o" "fluffbase_jnt.tx";
connectAttr "fluffbase_jnt_translateY.o" "fluffbase_jnt.ty";
connectAttr "fluffbase_jnt_translateZ.o" "fluffbase_jnt.tz";
connectAttr "fluffbase_jnt_rotateX.o" "fluffbase_jnt.rx";
connectAttr "fluffbase_jnt_rotateY.o" "fluffbase_jnt.ry";
connectAttr "fluffbase_jnt_rotateZ.o" "fluffbase_jnt.rz";
connectAttr "fluffbase_jnt_scaleX.o" "fluffbase_jnt.sx";
connectAttr "fluffbase_jnt_scaleY.o" "fluffbase_jnt.sy";
connectAttr "fluffbase_jnt_scaleZ.o" "fluffbase_jnt.sz";
connectAttr "fluffbase_jnt_visibility.o" "fluffbase_jnt.v";
connectAttr "fluffbase_jnt.s" "flufftip_jnt.is";
connectAttr "flufftip_jnt_translateX.o" "flufftip_jnt.tx";
connectAttr "flufftip_jnt_translateY.o" "flufftip_jnt.ty";
connectAttr "flufftip_jnt_translateZ.o" "flufftip_jnt.tz";
connectAttr "flufftip_jnt_rotateX.o" "flufftip_jnt.rx";
connectAttr "flufftip_jnt_rotateY.o" "flufftip_jnt.ry";
connectAttr "flufftip_jnt_rotateZ.o" "flufftip_jnt.rz";
connectAttr "flufftip_jnt_scaleX.o" "flufftip_jnt.sx";
connectAttr "flufftip_jnt_scaleY.o" "flufftip_jnt.sy";
connectAttr "flufftip_jnt_scaleZ.o" "flufftip_jnt.sz";
connectAttr "flufftip_jnt_visibility.o" "flufftip_jnt.v";
connectAttr "head_jnt.s" "R_brow_jnt.is";
connectAttr "R_brow_jnt_translateX.o" "R_brow_jnt.tx";
connectAttr "R_brow_jnt_translateY.o" "R_brow_jnt.ty";
connectAttr "R_brow_jnt_translateZ.o" "R_brow_jnt.tz";
connectAttr "R_brow_jnt_rotateX.o" "R_brow_jnt.rx";
connectAttr "R_brow_jnt_rotateY.o" "R_brow_jnt.ry";
connectAttr "R_brow_jnt_rotateZ.o" "R_brow_jnt.rz";
connectAttr "R_brow_jnt_visibility.o" "R_brow_jnt.v";
connectAttr "R_brow_jnt_scaleX.o" "R_brow_jnt.sx";
connectAttr "R_brow_jnt_scaleY.o" "R_brow_jnt.sy";
connectAttr "R_brow_jnt_scaleZ.o" "R_brow_jnt.sz";
connectAttr "head_jnt.s" "R_cheek_jnt.is";
connectAttr "R_cheek_jnt_translateX.o" "R_cheek_jnt.tx";
connectAttr "R_cheek_jnt_translateY.o" "R_cheek_jnt.ty";
connectAttr "R_cheek_jnt_translateZ.o" "R_cheek_jnt.tz";
connectAttr "R_cheek_jnt_rotateX.o" "R_cheek_jnt.rx";
connectAttr "R_cheek_jnt_rotateY.o" "R_cheek_jnt.ry";
connectAttr "R_cheek_jnt_rotateZ.o" "R_cheek_jnt.rz";
connectAttr "R_cheek_jnt_scaleX.o" "R_cheek_jnt.sx";
connectAttr "R_cheek_jnt_scaleY.o" "R_cheek_jnt.sy";
connectAttr "R_cheek_jnt_scaleZ.o" "R_cheek_jnt.sz";
connectAttr "R_cheek_jnt_visibility.o" "R_cheek_jnt.v";
connectAttr "head_jnt.s" "R_upperlip1_jnt.is";
connectAttr "R_upperlip1_jnt_translateX.o" "R_upperlip1_jnt.tx";
connectAttr "R_upperlip1_jnt_translateY.o" "R_upperlip1_jnt.ty";
connectAttr "R_upperlip1_jnt_translateZ.o" "R_upperlip1_jnt.tz";
connectAttr "R_upperlip1_jnt_rotateX.o" "R_upperlip1_jnt.rx";
connectAttr "R_upperlip1_jnt_rotateY.o" "R_upperlip1_jnt.ry";
connectAttr "R_upperlip1_jnt_rotateZ.o" "R_upperlip1_jnt.rz";
connectAttr "R_upperlip1_jnt_visibility.o" "R_upperlip1_jnt.v";
connectAttr "R_upperlip1_jnt_scaleX.o" "R_upperlip1_jnt.sx";
connectAttr "R_upperlip1_jnt_scaleY.o" "R_upperlip1_jnt.sy";
connectAttr "R_upperlip1_jnt_scaleZ.o" "R_upperlip1_jnt.sz";
connectAttr "head_jnt.s" "R_upperlip2_jnt.is";
connectAttr "R_upperlip2_jnt_translateX.o" "R_upperlip2_jnt.tx";
connectAttr "R_upperlip2_jnt_translateY.o" "R_upperlip2_jnt.ty";
connectAttr "R_upperlip2_jnt_translateZ.o" "R_upperlip2_jnt.tz";
connectAttr "R_upperlip2_jnt_rotateX.o" "R_upperlip2_jnt.rx";
connectAttr "R_upperlip2_jnt_rotateY.o" "R_upperlip2_jnt.ry";
connectAttr "R_upperlip2_jnt_rotateZ.o" "R_upperlip2_jnt.rz";
connectAttr "R_upperlip2_jnt_visibility.o" "R_upperlip2_jnt.v";
connectAttr "R_upperlip2_jnt_scaleX.o" "R_upperlip2_jnt.sx";
connectAttr "R_upperlip2_jnt_scaleY.o" "R_upperlip2_jnt.sy";
connectAttr "R_upperlip2_jnt_scaleZ.o" "R_upperlip2_jnt.sz";
connectAttr "head_jnt.s" "R_lipcorner_jnt.is";
connectAttr "R_lipcorner_jnt_translateX.o" "R_lipcorner_jnt.tx";
connectAttr "R_lipcorner_jnt_translateY.o" "R_lipcorner_jnt.ty";
connectAttr "R_lipcorner_jnt_translateZ.o" "R_lipcorner_jnt.tz";
connectAttr "R_lipcorner_jnt_rotateX.o" "R_lipcorner_jnt.rx";
connectAttr "R_lipcorner_jnt_rotateY.o" "R_lipcorner_jnt.ry";
connectAttr "R_lipcorner_jnt_rotateZ.o" "R_lipcorner_jnt.rz";
connectAttr "R_lipcorner_jnt_visibility.o" "R_lipcorner_jnt.v";
connectAttr "R_lipcorner_jnt_scaleX.o" "R_lipcorner_jnt.sx";
connectAttr "R_lipcorner_jnt_scaleY.o" "R_lipcorner_jnt.sy";
connectAttr "R_lipcorner_jnt_scaleZ.o" "R_lipcorner_jnt.sz";
connectAttr "head_jnt.s" "upperlip_jnt.is";
connectAttr "upperlip_jnt_translateX.o" "upperlip_jnt.tx";
connectAttr "upperlip_jnt_translateY.o" "upperlip_jnt.ty";
connectAttr "upperlip_jnt_translateZ.o" "upperlip_jnt.tz";
connectAttr "upperlip_jnt_rotateX.o" "upperlip_jnt.rx";
connectAttr "upperlip_jnt_rotateY.o" "upperlip_jnt.ry";
connectAttr "upperlip_jnt_rotateZ.o" "upperlip_jnt.rz";
connectAttr "upperlip_jnt_visibility.o" "upperlip_jnt.v";
connectAttr "upperlip_jnt_scaleX.o" "upperlip_jnt.sx";
connectAttr "upperlip_jnt_scaleY.o" "upperlip_jnt.sy";
connectAttr "upperlip_jnt_scaleZ.o" "upperlip_jnt.sz";
connectAttr "torso_jnt.s" "L_scapula_jnt.is";
connectAttr "L_scapula_jnt_translateX.o" "L_scapula_jnt.tx";
connectAttr "L_scapula_jnt_translateY.o" "L_scapula_jnt.ty";
connectAttr "L_scapula_jnt_translateZ.o" "L_scapula_jnt.tz";
connectAttr "L_scapula_jnt_rotateX.o" "L_scapula_jnt.rx";
connectAttr "L_scapula_jnt_rotateY.o" "L_scapula_jnt.ry";
connectAttr "L_scapula_jnt_rotateZ.o" "L_scapula_jnt.rz";
connectAttr "L_scapula_jnt_scaleX.o" "L_scapula_jnt.sx";
connectAttr "L_scapula_jnt_scaleY.o" "L_scapula_jnt.sy";
connectAttr "L_scapula_jnt_scaleZ.o" "L_scapula_jnt.sz";
connectAttr "L_scapula_jnt_visibility.o" "L_scapula_jnt.v";
connectAttr "L_scapula_jnt.s" "L_F_shoulder_jnt.is";
connectAttr "L_F_shoulder_jnt_scaleX.o" "L_F_shoulder_jnt.sx";
connectAttr "L_F_shoulder_jnt_scaleY.o" "L_F_shoulder_jnt.sy";
connectAttr "L_F_shoulder_jnt_scaleZ.o" "L_F_shoulder_jnt.sz";
connectAttr "L_F_shoulder_jnt_translateX.o" "L_F_shoulder_jnt.tx";
connectAttr "L_F_shoulder_jnt_translateY.o" "L_F_shoulder_jnt.ty";
connectAttr "L_F_shoulder_jnt_translateZ.o" "L_F_shoulder_jnt.tz";
connectAttr "L_F_shoulder_jnt_rotateX.o" "L_F_shoulder_jnt.rx";
connectAttr "L_F_shoulder_jnt_rotateY.o" "L_F_shoulder_jnt.ry";
connectAttr "L_F_shoulder_jnt_rotateZ.o" "L_F_shoulder_jnt.rz";
connectAttr "L_F_shoulder_jnt_visibility.o" "L_F_shoulder_jnt.v";
connectAttr "L_F_shoulder_jnt.s" "L_F_elbow_jnt.is";
connectAttr "L_F_elbow_jnt_scaleX.o" "L_F_elbow_jnt.sx";
connectAttr "L_F_elbow_jnt_scaleY.o" "L_F_elbow_jnt.sy";
connectAttr "L_F_elbow_jnt_scaleZ.o" "L_F_elbow_jnt.sz";
connectAttr "L_F_elbow_jnt_rotateX.o" "L_F_elbow_jnt.rx";
connectAttr "L_F_elbow_jnt_rotateY.o" "L_F_elbow_jnt.ry";
connectAttr "L_F_elbow_jnt_rotateZ.o" "L_F_elbow_jnt.rz";
connectAttr "L_F_elbow_jnt_visibility.o" "L_F_elbow_jnt.v";
connectAttr "L_F_elbow_jnt_translateX.o" "L_F_elbow_jnt.tx";
connectAttr "L_F_elbow_jnt_translateY.o" "L_F_elbow_jnt.ty";
connectAttr "L_F_elbow_jnt_translateZ.o" "L_F_elbow_jnt.tz";
connectAttr "L_F_elbow_jnt.s" "L_F_ankle_jnt.is";
connectAttr "L_F_ankle_jnt_scaleX.o" "L_F_ankle_jnt.sx";
connectAttr "L_F_ankle_jnt_scaleY.o" "L_F_ankle_jnt.sy";
connectAttr "L_F_ankle_jnt_scaleZ.o" "L_F_ankle_jnt.sz";
connectAttr "L_F_ankle_jnt_translateX.o" "L_F_ankle_jnt.tx";
connectAttr "L_F_ankle_jnt_translateY.o" "L_F_ankle_jnt.ty";
connectAttr "L_F_ankle_jnt_translateZ.o" "L_F_ankle_jnt.tz";
connectAttr "L_F_ankle_jnt_rotateX.o" "L_F_ankle_jnt.rx";
connectAttr "L_F_ankle_jnt_rotateY.o" "L_F_ankle_jnt.ry";
connectAttr "L_F_ankle_jnt_rotateZ.o" "L_F_ankle_jnt.rz";
connectAttr "L_F_ankle_jnt_visibility.o" "L_F_ankle_jnt.v";
connectAttr "L_F_ankle_jnt.s" "L_F_toe_jnt.is";
connectAttr "L_F_toe_jnt_translateX.o" "L_F_toe_jnt.tx";
connectAttr "L_F_toe_jnt_translateY.o" "L_F_toe_jnt.ty";
connectAttr "L_F_toe_jnt_translateZ.o" "L_F_toe_jnt.tz";
connectAttr "L_F_toe_jnt_visibility.o" "L_F_toe_jnt.v";
connectAttr "L_F_toe_jnt_rotateX.o" "L_F_toe_jnt.rx";
connectAttr "L_F_toe_jnt_rotateY.o" "L_F_toe_jnt.ry";
connectAttr "L_F_toe_jnt_rotateZ.o" "L_F_toe_jnt.rz";
connectAttr "L_F_toe_jnt_scaleX.o" "L_F_toe_jnt.sx";
connectAttr "L_F_toe_jnt_scaleY.o" "L_F_toe_jnt.sy";
connectAttr "L_F_toe_jnt_scaleZ.o" "L_F_toe_jnt.sz";
connectAttr "torso_jnt.s" "R_scapula_jnt.is";
connectAttr "R_scapula_jnt_translateX.o" "R_scapula_jnt.tx";
connectAttr "R_scapula_jnt_translateY.o" "R_scapula_jnt.ty";
connectAttr "R_scapula_jnt_translateZ.o" "R_scapula_jnt.tz";
connectAttr "R_scapula_jnt_rotateX.o" "R_scapula_jnt.rx";
connectAttr "R_scapula_jnt_rotateY.o" "R_scapula_jnt.ry";
connectAttr "R_scapula_jnt_rotateZ.o" "R_scapula_jnt.rz";
connectAttr "R_scapula_jnt_scaleX.o" "R_scapula_jnt.sx";
connectAttr "R_scapula_jnt_scaleY.o" "R_scapula_jnt.sy";
connectAttr "R_scapula_jnt_scaleZ.o" "R_scapula_jnt.sz";
connectAttr "R_scapula_jnt_visibility.o" "R_scapula_jnt.v";
connectAttr "R_scapula_jnt.s" "R_F_shoulder_jnt.is";
connectAttr "R_F_shoulder_jnt_scaleX.o" "R_F_shoulder_jnt.sx";
connectAttr "R_F_shoulder_jnt_scaleY.o" "R_F_shoulder_jnt.sy";
connectAttr "R_F_shoulder_jnt_scaleZ.o" "R_F_shoulder_jnt.sz";
connectAttr "R_F_shoulder_jnt_translateX.o" "R_F_shoulder_jnt.tx";
connectAttr "R_F_shoulder_jnt_translateY.o" "R_F_shoulder_jnt.ty";
connectAttr "R_F_shoulder_jnt_translateZ.o" "R_F_shoulder_jnt.tz";
connectAttr "R_F_shoulder_jnt_rotateX.o" "R_F_shoulder_jnt.rx";
connectAttr "R_F_shoulder_jnt_rotateY.o" "R_F_shoulder_jnt.ry";
connectAttr "R_F_shoulder_jnt_rotateZ.o" "R_F_shoulder_jnt.rz";
connectAttr "R_F_shoulder_jnt_visibility.o" "R_F_shoulder_jnt.v";
connectAttr "R_F_shoulder_jnt.s" "R_F_elbow_jnt.is";
connectAttr "R_F_elbow_jnt_scaleX.o" "R_F_elbow_jnt.sx";
connectAttr "R_F_elbow_jnt_scaleY.o" "R_F_elbow_jnt.sy";
connectAttr "R_F_elbow_jnt_scaleZ.o" "R_F_elbow_jnt.sz";
connectAttr "R_F_elbow_jnt_rotateX.o" "R_F_elbow_jnt.rx";
connectAttr "R_F_elbow_jnt_rotateY.o" "R_F_elbow_jnt.ry";
connectAttr "R_F_elbow_jnt_rotateZ.o" "R_F_elbow_jnt.rz";
connectAttr "R_F_elbow_jnt_visibility.o" "R_F_elbow_jnt.v";
connectAttr "R_F_elbow_jnt_translateX.o" "R_F_elbow_jnt.tx";
connectAttr "R_F_elbow_jnt_translateY.o" "R_F_elbow_jnt.ty";
connectAttr "R_F_elbow_jnt_translateZ.o" "R_F_elbow_jnt.tz";
connectAttr "R_F_elbow_jnt.s" "R_F_ankle_jnt.is";
connectAttr "R_F_ankle_jnt_scaleX.o" "R_F_ankle_jnt.sx";
connectAttr "R_F_ankle_jnt_scaleY.o" "R_F_ankle_jnt.sy";
connectAttr "R_F_ankle_jnt_scaleZ.o" "R_F_ankle_jnt.sz";
connectAttr "R_F_ankle_jnt_translateX.o" "R_F_ankle_jnt.tx";
connectAttr "R_F_ankle_jnt_translateY.o" "R_F_ankle_jnt.ty";
connectAttr "R_F_ankle_jnt_translateZ.o" "R_F_ankle_jnt.tz";
connectAttr "R_F_ankle_jnt_rotateX.o" "R_F_ankle_jnt.rx";
connectAttr "R_F_ankle_jnt_rotateY.o" "R_F_ankle_jnt.ry";
connectAttr "R_F_ankle_jnt_rotateZ.o" "R_F_ankle_jnt.rz";
connectAttr "R_F_ankle_jnt_visibility.o" "R_F_ankle_jnt.v";
connectAttr "R_F_ankle_jnt.s" "R_F_toe_jnt.is";
connectAttr "R_F_toe_jnt_translateX.o" "R_F_toe_jnt.tx";
connectAttr "R_F_toe_jnt_translateY.o" "R_F_toe_jnt.ty";
connectAttr "R_F_toe_jnt_translateZ.o" "R_F_toe_jnt.tz";
connectAttr "R_F_toe_jnt_visibility.o" "R_F_toe_jnt.v";
connectAttr "R_F_toe_jnt_rotateX.o" "R_F_toe_jnt.rx";
connectAttr "R_F_toe_jnt_rotateY.o" "R_F_toe_jnt.ry";
connectAttr "R_F_toe_jnt_rotateZ.o" "R_F_toe_jnt.rz";
connectAttr "R_F_toe_jnt_scaleX.o" "R_F_toe_jnt.sx";
connectAttr "R_F_toe_jnt_scaleY.o" "R_F_toe_jnt.sy";
connectAttr "R_F_toe_jnt_scaleZ.o" "R_F_toe_jnt.sz";
connectAttr "COG_jnt.s" "torso_splineik_jnt.is";
connectAttr "torso_splineik_jnt_translateX.o" "torso_splineik_jnt.tx";
connectAttr "torso_splineik_jnt_translateY.o" "torso_splineik_jnt.ty";
connectAttr "torso_splineik_jnt_translateZ.o" "torso_splineik_jnt.tz";
connectAttr "torso_splineik_jnt_rotateX.o" "torso_splineik_jnt.rx";
connectAttr "torso_splineik_jnt_rotateY.o" "torso_splineik_jnt.ry";
connectAttr "torso_splineik_jnt_rotateZ.o" "torso_splineik_jnt.rz";
connectAttr "torso_splineik_jnt_visibility.o" "torso_splineik_jnt.v";
connectAttr "torso_splineik_jnt_scaleX.o" "torso_splineik_jnt.sx";
connectAttr "torso_splineik_jnt_scaleY.o" "torso_splineik_jnt.sy";
connectAttr "torso_splineik_jnt_scaleZ.o" "torso_splineik_jnt.sz";
connectAttr "COG_jnt.s" "pelvis_splineik_jnt.is";
connectAttr "pelvis_splineik_jnt_translateX.o" "pelvis_splineik_jnt.tx";
connectAttr "pelvis_splineik_jnt_translateY.o" "pelvis_splineik_jnt.ty";
connectAttr "pelvis_splineik_jnt_translateZ.o" "pelvis_splineik_jnt.tz";
connectAttr "pelvis_splineik_jnt_rotateX.o" "pelvis_splineik_jnt.rx";
connectAttr "pelvis_splineik_jnt_rotateY.o" "pelvis_splineik_jnt.ry";
connectAttr "pelvis_splineik_jnt_rotateZ.o" "pelvis_splineik_jnt.rz";
connectAttr "pelvis_splineik_jnt_visibility.o" "pelvis_splineik_jnt.v";
connectAttr "pelvis_splineik_jnt_scaleX.o" "pelvis_splineik_jnt.sx";
connectAttr "pelvis_splineik_jnt_scaleY.o" "pelvis_splineik_jnt.sy";
connectAttr "pelvis_splineik_jnt_scaleZ.o" "pelvis_splineik_jnt.sz";
relationship "link" ":lightLinker1" ":initialShadingGroup.message" ":defaultLightSet.message";
relationship "link" ":lightLinker1" ":initialParticleSE.message" ":defaultLightSet.message";
relationship "link" ":lightLinker1" "bodySG.message" ":defaultLightSet.message";
relationship "link" ":lightLinker1" "Ha_highSG.message" ":defaultLightSet.message";
relationship "link" ":lightLinker1" "EyebrowsSG.message" ":defaultLightSet.message";
relationship "link" ":lightLinker1" "RightOuterEye_lowSG.message" ":defaultLightSet.message";
relationship "link" ":lightLinker1" "RightEye_lowSG.message" ":defaultLightSet.message";
relationship "link" ":lightLinker1" "LeftOuterEye_low001SG.message" ":defaultLightSet.message";
relationship "link" ":lightLinker1" "LeftEye_lowSG.message" ":defaultLightSet.message";
relationship "link" ":lightLinker1" "bodySG1.message" ":defaultLightSet.message";
relationship "link" ":lightLinker1" "bodySG2.message" ":defaultLightSet.message";
relationship "link" ":lightLinker1" "fluff_lowSG.message" ":defaultLightSet.message";
relationship "link" ":lightLinker1" "body_low001SG.message" ":defaultLightSet.message";
relationship "link" ":lightLinker1" "ballmonster_lowSG.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" ":initialShadingGroup.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" ":initialParticleSE.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" "bodySG.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" "Ha_highSG.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" "EyebrowsSG.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" "RightOuterEye_lowSG.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" "RightEye_lowSG.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" "LeftOuterEye_low001SG.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" "LeftEye_lowSG.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" "bodySG1.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" "bodySG2.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" "fluff_lowSG.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" "body_low001SG.message" ":defaultLightSet.message";
relationship "shadowLink" ":lightLinker1" "ballmonster_lowSG.message" ":defaultLightSet.message";
connectAttr "layerManager.dli[0]" "defaultLayer.id";
connectAttr "renderLayerManager.rlmi[0]" "defaultRenderLayer.rlid";
connectAttr "MaterialFBXASC032FBXASC03550.oc" "bodySG.ss";
connectAttr "bodySG.msg" "materialInfo1.sg";
connectAttr "MaterialFBXASC032FBXASC03550.msg" "materialInfo1.m";
connectAttr "layerManager.dli[1]" "mesh.id";
connectAttr "layerManager.dli[3]" "control.id";
connectAttr "FBXASC0500FBXASC032FBXASC045FBXASC032Default.oc" "Ha_highSG.ss";
connectAttr "Ha_highSG.msg" "materialInfo2.sg";
connectAttr "FBXASC0500FBXASC032FBXASC045FBXASC032Default.msg" "materialInfo2.m"
		;
connectAttr "Hair.oc" "EyebrowsSG.ss";
connectAttr "EyebrowsSG.msg" "materialInfo3.sg";
connectAttr "Hair.msg" "materialInfo3.m";
connectAttr "OuterFBXASC032EyeFBXASC032Glass.out" "RightOuterEye_lowSG.ss";
connectAttr "RightOuterEye_lowSG.msg" "materialInfo4.sg";
connectAttr "OuterFBXASC032EyeFBXASC032Glass.msg" "materialInfo4.m";
connectAttr "OuterFBXASC032EyeFBXASC032Glass.msg" "materialInfo4.t" -na;
connectAttr "Char_BaseColor.oc" "CharacterFBXASC032Surface.base_color";
connectAttr "Char_Emissive.oc" "CharacterFBXASC032Surface.emission_color";
connectAttr "place2dTexture1.o" "Char_BaseColor.uv";
connectAttr "place2dTexture1.ofu" "Char_BaseColor.ofu";
connectAttr "place2dTexture1.ofv" "Char_BaseColor.ofv";
connectAttr "place2dTexture1.rf" "Char_BaseColor.rf";
connectAttr "place2dTexture1.reu" "Char_BaseColor.reu";
connectAttr "place2dTexture1.rev" "Char_BaseColor.rev";
connectAttr "place2dTexture1.vt1" "Char_BaseColor.vt1";
connectAttr "place2dTexture1.vt2" "Char_BaseColor.vt2";
connectAttr "place2dTexture1.vt3" "Char_BaseColor.vt3";
connectAttr "place2dTexture1.vc1" "Char_BaseColor.vc1";
connectAttr "place2dTexture1.ofs" "Char_BaseColor.fs";
connectAttr ":defaultColorMgtGlobals.cme" "Char_BaseColor.cme";
connectAttr ":defaultColorMgtGlobals.cfe" "Char_BaseColor.cmcf";
connectAttr ":defaultColorMgtGlobals.cfp" "Char_BaseColor.cmcp";
connectAttr ":defaultColorMgtGlobals.wsn" "Char_BaseColor.ws";
connectAttr "place2dTexture2.o" "Char_Roughness.uv";
connectAttr "place2dTexture2.ofu" "Char_Roughness.ofu";
connectAttr "place2dTexture2.ofv" "Char_Roughness.ofv";
connectAttr "place2dTexture2.rf" "Char_Roughness.rf";
connectAttr "place2dTexture2.reu" "Char_Roughness.reu";
connectAttr "place2dTexture2.rev" "Char_Roughness.rev";
connectAttr "place2dTexture2.vt1" "Char_Roughness.vt1";
connectAttr "place2dTexture2.vt2" "Char_Roughness.vt2";
connectAttr "place2dTexture2.vt3" "Char_Roughness.vt3";
connectAttr "place2dTexture2.vc1" "Char_Roughness.vc1";
connectAttr "place2dTexture2.ofs" "Char_Roughness.fs";
connectAttr ":defaultColorMgtGlobals.cme" "Char_Roughness.cme";
connectAttr ":defaultColorMgtGlobals.cfe" "Char_Roughness.cmcf";
connectAttr ":defaultColorMgtGlobals.cfp" "Char_Roughness.cmcp";
connectAttr ":defaultColorMgtGlobals.wsn" "Char_Roughness.ws";
connectAttr "place2dTexture3.o" "Char_Emissive.uv";
connectAttr "place2dTexture3.ofu" "Char_Emissive.ofu";
connectAttr "place2dTexture3.ofv" "Char_Emissive.ofv";
connectAttr "place2dTexture3.rf" "Char_Emissive.rf";
connectAttr "place2dTexture3.reu" "Char_Emissive.reu";
connectAttr "place2dTexture3.rev" "Char_Emissive.rev";
connectAttr "place2dTexture3.vt1" "Char_Emissive.vt1";
connectAttr "place2dTexture3.vt2" "Char_Emissive.vt2";
connectAttr "place2dTexture3.vt3" "Char_Emissive.vt3";
connectAttr "place2dTexture3.vc1" "Char_Emissive.vc1";
connectAttr "place2dTexture3.ofs" "Char_Emissive.fs";
connectAttr ":defaultColorMgtGlobals.cme" "Char_Emissive.cme";
connectAttr ":defaultColorMgtGlobals.cfe" "Char_Emissive.cmcf";
connectAttr ":defaultColorMgtGlobals.cfp" "Char_Emissive.cmcp";
connectAttr ":defaultColorMgtGlobals.wsn" "Char_Emissive.ws";
connectAttr "CharacterFBXASC032Surface.out" "RightEye_lowSG.ss";
connectAttr "RightEye_lowSG.msg" "materialInfo5.sg";
connectAttr "CharacterFBXASC032Surface.msg" "materialInfo5.m";
connectAttr "CharacterFBXASC032Surface.msg" "materialInfo5.t" -na;
connectAttr "OuterFBXASC032EyeFBXASC032Glass.out" "LeftOuterEye_low001SG.ss";
connectAttr "LeftOuterEye_low001SG.msg" "materialInfo6.sg";
connectAttr "OuterFBXASC032EyeFBXASC032Glass.msg" "materialInfo6.m";
connectAttr "OuterFBXASC032EyeFBXASC032Glass.msg" "materialInfo6.t" -na;
connectAttr "CharacterFBXASC032Surface.out" "LeftEye_lowSG.ss";
connectAttr "LeftEye_lowSG.msg" "materialInfo7.sg";
connectAttr "CharacterFBXASC032Surface.msg" "materialInfo7.m";
connectAttr "CharacterFBXASC032Surface.msg" "materialInfo7.t" -na;
connectAttr "MaterialFBXASC032FBXASC03551.oc" "bodySG1.ss";
connectAttr "bodySG1.msg" "materialInfo8.sg";
connectAttr "MaterialFBXASC032FBXASC03551.msg" "materialInfo8.m";
connectAttr "MaterialFBXASC032FBXASC03552.oc" "bodySG2.ss";
connectAttr "bodySG2.msg" "materialInfo9.sg";
connectAttr "MaterialFBXASC032FBXASC03552.msg" "materialInfo9.m";
connectAttr "MaterialFBXASC032FBXASC03553.oc" "fluff_lowSG.ss";
connectAttr "fluff_lowSG.msg" "materialInfo10.sg";
connectAttr "MaterialFBXASC032FBXASC03553.msg" "materialInfo10.m";
connectAttr "MaterialFBXASC032FBXASC03554.oc" "body_low001SG.ss";
connectAttr "body_low001SG.msg" "materialInfo11.sg";
connectAttr "MaterialFBXASC032FBXASC03554.msg" "materialInfo11.m";
connectAttr ":defaultArnoldDisplayDriver.msg" ":defaultArnoldRenderOptions.drivers"
		 -na;
connectAttr ":defaultArnoldFilter.msg" ":defaultArnoldRenderOptions.filt";
connectAttr ":defaultArnoldDriver.msg" ":defaultArnoldRenderOptions.drvr";
connectAttr "MaterialFBXASC032FBXASC03555.oc" "ballmonster_lowSG.ss";
connectAttr "ballmonster_lowSG.msg" "materialInfo12.sg";
connectAttr "MaterialFBXASC032FBXASC03555.msg" "materialInfo12.m";
connectAttr "joints_grp.msg" "bindPose1.m[0]";
connectAttr "BallMonster_root_jnt.msg" "bindPose1.m[1]";
connectAttr "COG_jnt.msg" "bindPose1.m[2]";
connectAttr "torso_jnt.msg" "bindPose1.m[3]";
connectAttr "spine2_jnt.msg" "bindPose1.m[4]";
connectAttr "spine1_jnt.msg" "bindPose1.m[5]";
connectAttr "pelvis_jnt.msg" "bindPose1.m[6]";
connectAttr "tailbase_jnt.msg" "bindPose1.m[7]";
connectAttr "tail_jnt.msg" "bindPose1.m[8]";
connectAttr "L_B_hip_jnt.msg" "bindPose1.m[9]";
connectAttr "L_B_knee_jnt.msg" "bindPose1.m[10]";
connectAttr "L_B_ankle_jnt.msg" "bindPose1.m[11]";
connectAttr "L_B_toe_jnt.msg" "bindPose1.m[12]";
connectAttr "R_B_hip_jnt.msg" "bindPose1.m[13]";
connectAttr "R_B_knee_jnt.msg" "bindPose1.m[14]";
connectAttr "R_B_ankle_jnt.msg" "bindPose1.m[15]";
connectAttr "R_B_toe_jnt.msg" "bindPose1.m[16]";
connectAttr "head_jnt.msg" "bindPose1.m[17]";
connectAttr "jaw_jnt.msg" "bindPose1.m[18]";
connectAttr "tonguebase_jnt.msg" "bindPose1.m[19]";
connectAttr "tonguemid_jnt.msg" "bindPose1.m[20]";
connectAttr "tonguetip_jnt.msg" "bindPose1.m[21]";
connectAttr "L_lowerlip2_jnt.msg" "bindPose1.m[22]";
connectAttr "L_lowerlip1_jnt.msg" "bindPose1.m[23]";
connectAttr "lowerlip.msg" "bindPose1.m[24]";
connectAttr "R_lowerlip2_jnt.msg" "bindPose1.m[25]";
connectAttr "R_lowerlip1_jnt.msg" "bindPose1.m[26]";
connectAttr "L_upperlip1_jnt.msg" "bindPose1.m[28]";
connectAttr "L_upperlip2_jnt.msg" "bindPose1.m[29]";
connectAttr "L_lipcorner_jnt.msg" "bindPose1.m[30]";
connectAttr "L_cheek_jnt.msg" "bindPose1.m[31]";
connectAttr "L_brow_jnt.msg" "bindPose1.m[32]";
connectAttr "browmid_jnt.msg" "bindPose1.m[33]";
connectAttr "fluffbase_jnt.msg" "bindPose1.m[34]";
connectAttr "flufftip_jnt.msg" "bindPose1.m[35]";
connectAttr "R_brow_jnt.msg" "bindPose1.m[36]";
connectAttr "R_cheek_jnt.msg" "bindPose1.m[37]";
connectAttr "R_upperlip1_jnt.msg" "bindPose1.m[38]";
connectAttr "R_upperlip2_jnt.msg" "bindPose1.m[39]";
connectAttr "R_lipcorner_jnt.msg" "bindPose1.m[40]";
connectAttr "L_scapula_jnt.msg" "bindPose1.m[41]";
connectAttr "L_F_shoulder_jnt.msg" "bindPose1.m[42]";
connectAttr "L_F_elbow_jnt.msg" "bindPose1.m[43]";
connectAttr "L_F_ankle_jnt.msg" "bindPose1.m[44]";
connectAttr "L_F_toe_jnt.msg" "bindPose1.m[45]";
connectAttr "R_scapula_jnt.msg" "bindPose1.m[46]";
connectAttr "R_F_shoulder_jnt.msg" "bindPose1.m[47]";
connectAttr "R_F_elbow_jnt.msg" "bindPose1.m[48]";
connectAttr "R_F_ankle_jnt.msg" "bindPose1.m[49]";
connectAttr "R_F_toe_jnt.msg" "bindPose1.m[50]";
connectAttr "upperlip_jnt.msg" "bindPose1.m[51]";
connectAttr "bindPose1.w" "bindPose1.p[0]";
connectAttr "bindPose1.m[0]" "bindPose1.p[1]";
connectAttr "bindPose1.m[1]" "bindPose1.p[2]";
connectAttr "bindPose1.m[2]" "bindPose1.p[3]";
connectAttr "bindPose1.m[3]" "bindPose1.p[4]";
connectAttr "bindPose1.m[4]" "bindPose1.p[5]";
connectAttr "bindPose1.m[5]" "bindPose1.p[6]";
connectAttr "bindPose1.m[6]" "bindPose1.p[7]";
connectAttr "bindPose1.m[7]" "bindPose1.p[8]";
connectAttr "bindPose1.m[6]" "bindPose1.p[9]";
connectAttr "bindPose1.m[9]" "bindPose1.p[10]";
connectAttr "bindPose1.m[10]" "bindPose1.p[11]";
connectAttr "bindPose1.m[11]" "bindPose1.p[12]";
connectAttr "bindPose1.m[6]" "bindPose1.p[13]";
connectAttr "bindPose1.m[13]" "bindPose1.p[14]";
connectAttr "bindPose1.m[14]" "bindPose1.p[15]";
connectAttr "bindPose1.m[15]" "bindPose1.p[16]";
connectAttr "bindPose1.m[3]" "bindPose1.p[17]";
connectAttr "bindPose1.m[17]" "bindPose1.p[18]";
connectAttr "bindPose1.m[18]" "bindPose1.p[19]";
connectAttr "bindPose1.m[19]" "bindPose1.p[20]";
connectAttr "bindPose1.m[20]" "bindPose1.p[21]";
connectAttr "bindPose1.m[18]" "bindPose1.p[22]";
connectAttr "bindPose1.m[18]" "bindPose1.p[23]";
connectAttr "bindPose1.m[18]" "bindPose1.p[24]";
connectAttr "bindPose1.m[18]" "bindPose1.p[25]";
connectAttr "bindPose1.m[18]" "bindPose1.p[26]";
connectAttr "bindPose1.m[17]" "bindPose1.p[28]";
connectAttr "bindPose1.m[17]" "bindPose1.p[29]";
connectAttr "bindPose1.m[17]" "bindPose1.p[30]";
connectAttr "bindPose1.m[17]" "bindPose1.p[31]";
connectAttr "bindPose1.m[17]" "bindPose1.p[32]";
connectAttr "bindPose1.m[17]" "bindPose1.p[33]";
connectAttr "bindPose1.m[17]" "bindPose1.p[34]";
connectAttr "bindPose1.m[34]" "bindPose1.p[35]";
connectAttr "bindPose1.m[17]" "bindPose1.p[36]";
connectAttr "bindPose1.m[17]" "bindPose1.p[37]";
connectAttr "bindPose1.m[17]" "bindPose1.p[38]";
connectAttr "bindPose1.m[17]" "bindPose1.p[39]";
connectAttr "bindPose1.m[17]" "bindPose1.p[40]";
connectAttr "bindPose1.m[3]" "bindPose1.p[41]";
connectAttr "bindPose1.m[41]" "bindPose1.p[42]";
connectAttr "bindPose1.m[42]" "bindPose1.p[43]";
connectAttr "bindPose1.m[43]" "bindPose1.p[44]";
connectAttr "bindPose1.m[44]" "bindPose1.p[45]";
connectAttr "bindPose1.m[3]" "bindPose1.p[46]";
connectAttr "bindPose1.m[46]" "bindPose1.p[47]";
connectAttr "bindPose1.m[47]" "bindPose1.p[48]";
connectAttr "bindPose1.m[48]" "bindPose1.p[49]";
connectAttr "bindPose1.m[49]" "bindPose1.p[50]";
connectAttr "bindPose1.m[17]" "bindPose1.p[51]";
connectAttr "BallMonster_root_jnt.bps" "bindPose1.wm[1]";
connectAttr "COG_jnt.bps" "bindPose1.wm[2]";
connectAttr "torso_jnt.bps" "bindPose1.wm[3]";
connectAttr "spine2_jnt.bps" "bindPose1.wm[4]";
connectAttr "spine1_jnt.bps" "bindPose1.wm[5]";
connectAttr "pelvis_jnt.bps" "bindPose1.wm[6]";
connectAttr "tailbase_jnt.bps" "bindPose1.wm[7]";
connectAttr "tail_jnt.bps" "bindPose1.wm[8]";
connectAttr "L_B_hip_jnt.bps" "bindPose1.wm[9]";
connectAttr "L_B_knee_jnt.bps" "bindPose1.wm[10]";
connectAttr "L_B_ankle_jnt.bps" "bindPose1.wm[11]";
connectAttr "L_B_toe_jnt.bps" "bindPose1.wm[12]";
connectAttr "R_B_hip_jnt.bps" "bindPose1.wm[13]";
connectAttr "R_B_knee_jnt.bps" "bindPose1.wm[14]";
connectAttr "R_B_ankle_jnt.bps" "bindPose1.wm[15]";
connectAttr "R_B_toe_jnt.bps" "bindPose1.wm[16]";
connectAttr "head_jnt.bps" "bindPose1.wm[17]";
connectAttr "jaw_jnt.bps" "bindPose1.wm[18]";
connectAttr "tonguebase_jnt.bps" "bindPose1.wm[19]";
connectAttr "tonguemid_jnt.bps" "bindPose1.wm[20]";
connectAttr "tonguetip_jnt.bps" "bindPose1.wm[21]";
connectAttr "L_lowerlip2_jnt.bps" "bindPose1.wm[22]";
connectAttr "L_lowerlip1_jnt.bps" "bindPose1.wm[23]";
connectAttr "lowerlip.bps" "bindPose1.wm[24]";
connectAttr "R_lowerlip2_jnt.bps" "bindPose1.wm[25]";
connectAttr "R_lowerlip1_jnt.bps" "bindPose1.wm[26]";
connectAttr "L_upperlip1_jnt.bps" "bindPose1.wm[28]";
connectAttr "L_upperlip2_jnt.bps" "bindPose1.wm[29]";
connectAttr "L_lipcorner_jnt.bps" "bindPose1.wm[30]";
connectAttr "L_cheek_jnt.bps" "bindPose1.wm[31]";
connectAttr "L_brow_jnt.bps" "bindPose1.wm[32]";
connectAttr "browmid_jnt.bps" "bindPose1.wm[33]";
connectAttr "fluffbase_jnt.bps" "bindPose1.wm[34]";
connectAttr "flufftip_jnt.bps" "bindPose1.wm[35]";
connectAttr "R_brow_jnt.bps" "bindPose1.wm[36]";
connectAttr "R_cheek_jnt.bps" "bindPose1.wm[37]";
connectAttr "R_upperlip1_jnt.bps" "bindPose1.wm[38]";
connectAttr "R_upperlip2_jnt.bps" "bindPose1.wm[39]";
connectAttr "R_lipcorner_jnt.bps" "bindPose1.wm[40]";
connectAttr "L_scapula_jnt.bps" "bindPose1.wm[41]";
connectAttr "L_F_shoulder_jnt.bps" "bindPose1.wm[42]";
connectAttr "L_F_elbow_jnt.bps" "bindPose1.wm[43]";
connectAttr "L_F_ankle_jnt.bps" "bindPose1.wm[44]";
connectAttr "L_F_toe_jnt.bps" "bindPose1.wm[45]";
connectAttr "R_scapula_jnt.bps" "bindPose1.wm[46]";
connectAttr "R_F_shoulder_jnt.bps" "bindPose1.wm[47]";
connectAttr "R_F_elbow_jnt.bps" "bindPose1.wm[48]";
connectAttr "R_F_ankle_jnt.bps" "bindPose1.wm[49]";
connectAttr "R_F_toe_jnt.bps" "bindPose1.wm[50]";
connectAttr "upperlip_jnt.bps" "bindPose1.wm[51]";
connectAttr "layerManager.dli[2]" "joints.id";
connectAttr "torso_splineik_jnt.msg" "bindPose2.m[0]";
connectAttr "pelvis_splineik_jnt.msg" "bindPose2.m[1]";
connectAttr "bindPose2.w" "bindPose2.p[0]";
connectAttr "bindPose2.w" "bindPose2.p[1]";
connectAttr "torso_splineik_jnt.bps" "bindPose2.wm[0]";
connectAttr "pelvis_splineik_jnt.bps" "bindPose2.wm[1]";
connectAttr "bodySG.pa" ":renderPartition.st" -na;
connectAttr "Ha_highSG.pa" ":renderPartition.st" -na;
connectAttr "EyebrowsSG.pa" ":renderPartition.st" -na;
connectAttr "RightOuterEye_lowSG.pa" ":renderPartition.st" -na;
connectAttr "RightEye_lowSG.pa" ":renderPartition.st" -na;
connectAttr "LeftOuterEye_low001SG.pa" ":renderPartition.st" -na;
connectAttr "LeftEye_lowSG.pa" ":renderPartition.st" -na;
connectAttr "bodySG1.pa" ":renderPartition.st" -na;
connectAttr "bodySG2.pa" ":renderPartition.st" -na;
connectAttr "fluff_lowSG.pa" ":renderPartition.st" -na;
connectAttr "body_low001SG.pa" ":renderPartition.st" -na;
connectAttr "ballmonster_lowSG.pa" ":renderPartition.st" -na;
connectAttr "MaterialFBXASC032FBXASC03550.msg" ":defaultShaderList1.s" -na;
connectAttr "FBXASC0500FBXASC032FBXASC045FBXASC032Default.msg" ":defaultShaderList1.s"
		 -na;
connectAttr "Hair.msg" ":defaultShaderList1.s" -na;
connectAttr "OuterFBXASC032EyeFBXASC032Glass.msg" ":defaultShaderList1.s" -na;
connectAttr "CharacterFBXASC032Surface.msg" ":defaultShaderList1.s" -na;
connectAttr "MaterialFBXASC032FBXASC03551.msg" ":defaultShaderList1.s" -na;
connectAttr "MaterialFBXASC032FBXASC03552.msg" ":defaultShaderList1.s" -na;
connectAttr "MaterialFBXASC032FBXASC03553.msg" ":defaultShaderList1.s" -na;
connectAttr "MaterialFBXASC032FBXASC03554.msg" ":defaultShaderList1.s" -na;
connectAttr "MaterialFBXASC032FBXASC03555.msg" ":defaultShaderList1.s" -na;
connectAttr "place2dTexture1.msg" ":defaultRenderUtilityList1.u" -na;
connectAttr "place2dTexture2.msg" ":defaultRenderUtilityList1.u" -na;
connectAttr "place2dTexture3.msg" ":defaultRenderUtilityList1.u" -na;
connectAttr "defaultRenderLayer.msg" ":defaultRenderingList1.r" -na;
connectAttr "Char_BaseColor.msg" ":defaultTextureList1.tx" -na;
connectAttr "Char_Roughness.msg" ":defaultTextureList1.tx" -na;
connectAttr "Char_Emissive.msg" ":defaultTextureList1.tx" -na;
// End of ball_monster_maya_idle.ma
