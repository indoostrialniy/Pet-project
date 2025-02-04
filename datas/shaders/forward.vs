#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aIndex;
layout (location = 4) in vec4 B1B2;
layout (location = 5) in vec4 B3B4;
layout (location = 6) in vec4 B5B6;
layout (location = 7) in vec2 B7;


layout(binding = 8, std140) uniform PVM_Matrices
{
    mat4 PV;
    mat4 M;
    vec4 CameraPosition;
    vec4 CameraDirection;
};

layout(binding = 9, std140) uniform SKELETON_UBO
{

    mat4 IERARCHY[70];              //массив матриц, хранящих цепочку из максимум 16 родительских костей. 70 костей максимум - этого хватит с запасом для любого скелета. Каждое значение инкремментировано!
    mat4 TRANSFORMINTO[70];         //массив матриц, хранящих индексы матриц перехода в пространства соответствующих костей из Ierarchy
    mat4 TRANSFORMNEXT[70];         //массив матриц, хранящих индексы матриц перехода в пространства следующих соответствующих костей из Ierarchy
    mat4 LSK_Matrices[70];          //массив матриц ориентации костей в Т-позе
    mat4 TRANSFORMMATRICES[140];    //массив матриц переходов в пространства костей скелета. Размер = Nкостей * 2. Матрицы описывают ориентацию кости и координату ее "головы"
    mat4 POSES[70];               //массив матриц, хранящих кватернионов, описывающих ориентации костей в позе, а также смещение костей в позе и координаты головы кости в позе

};


uniform int skeletal;                   //флаг о том, что отрисовываемый объект - скелетный

out VS_OUT
{
    vec3 ourColor;
    vec2 TexCoord;
    float matIndex;
    vec4 Pos;
    vec4 Normal;
    mat3 txrxRotationMatrix;
    vec3 CameraPos;
    vec3 CameraDir;

} vs_out;


//a - xyzw, b- xyzw
vec4 CmulCXYZW(vec4 a, vec4 b)    //функция для перемножения комплексных чисел, например - кватернионов (в формате w i j k)
{
    //16m 12a
    return vec4(a[3]*b[0] + b[3]*a[0] + a[1]*b[2] - b[1]*a[2],
                a[3]*b[1] + b[3]*a[1] + a[2]*b[0] - b[2]*a[0],
                a[3]*b[2] + b[3]*a[2] + a[0]*b[1] - b[0]*a[1],
                a[3]*b[3] - a[0]*b[0] - a[1]*b[1] - a[2]*b[2]     );
}

vec4 RotByQuatXYZW(vec4 coordinate, vec4 quat)    //функция возвращает координаты вектора, измененные после применения кватерниона
{
    //32m 24a
    return CmulCXYZW( CmulCXYZW( quat , coordinate), vec4( -quat[0], -quat[1], -quat[2], quat[3] ) ); //
}



vec4 RotByQuatXYZW_2(vec4 u, vec4 l)    //функция возвращает координаты вектора u, повернутые кватернионом l, оба аргумента в формате XYZW (SK_Attachments_6.sm)
{
    //чуть более оптимизированная функция
    //12m 8a
    vec4 m = vec4(0); //xyzw
    m[0] =  u[0]*l[3] - u[1]*l[2] + u[2]*l[1];  //i
    m[1] =  u[0]*l[2] + u[1]*l[3] - u[2]*l[0];  //j
    m[2] = -u[0]*l[1] + u[1]*l[0] + u[2]*l[3];  //k
    m[3] =  u[0]*l[0] + u[1]*l[1] + u[2]*l[2]; //

    //12m 9a
    vec4 v = vec4(0); //xyzw
    v[0] = l[3]*m[0] + l[0]*m[3] + l[1]*m[2] - l[2]*m[1];   //i
    v[1] = l[3]*m[1] - l[0]*m[2] + l[1]*m[3] + l[2]*m[0];   //j
    v[2] = l[3]*m[2] + l[0]*m[1] - l[1]*m[0] + l[2]*m[3];   //k
    v[3] = 1;

    //24m 17a
    return v;
}


vec4 RotByQuatXYZW_3(vec4 u, vec4 l)    //функция возвращает координаты вектора u, повернутые кватернионом l, оба аргумента в формате XYZW (SK_Attachments_6.sm)
{
    //еще более оптимизированная функция - 18m 12a
    vec3 t = 2 * cross( l.xyz, u.xyz) ;
    vec3 v = u.xyz + l.w*t + cross(l.xyz, t);

    return vec4(v, 1);
}




vec4 rotate_point10(vec4 point, int bone_index, int transformMatrixIndex)
{
    mat4 toLocal = TRANSFORMMATRICES[transformMatrixIndex]; //матрица для пересчета в пространство текущей кости С УЧЕТОМ ПОЛОЖЕНИЯ ГОЛОВЫ В Т-ПОЗЕ!!!

    point = toLocal * point;

    if(bone_index >= 0)
    {
        mat4 QOH = POSES[bone_index];//POSES[bone_index];

        //здесь нужно получить кватернион для кости bone_index
        vec4 rotator = QOH[0]; // rotator считывается из файла анимации как vec4(xyzw)    vec4(0,0,0,1);//

        //if(bone_index < 2)
        //     rotator = vec4(0,0,0,1);

//         if(bone_index==27)
//             rotator = vec4(0.707, 0, 0, 0.707);

        vec4 myoffset = vec4(QOH[1].xyz, 0); //координаты смещения кости vec4(0,0,0,0);//

        //if( rotator.w < 0.999)
        {
            point = RotByQuatXYZW_3(vec4(point.xyz, 1), rotator);
        }

        point += myoffset;

    }




    return point; //vec4(p,1); //
}




vec4 calculateBranchRotation11(int bone_index, vec4 rotated_point)
{
    //на входе функции имеем координаты точки (в мире) и индекс кости, по цепи которой надо пройтись
    //надо определить цепь костей, включая эту



    //а если точка привязана к 0-й кости?


    // какая бы это ни была кость, сначала перейдем в ее пространство
    // первым этапом провернем по самой старшей кости и в качестве матрицы перехода ей укажем ВХОДНУЮ (TRANSFORMINTO)!
    int currentBoneIndex = bone_index; // циклом for перебираем кости ветви IERARCHY[bone_index]
    int currentBoneTransformSpaceMatrix     = int(TRANSFORMINTO[bone_index][0][0]); // получим индекс матрицы для перевода в систему кости

    rotated_point = rotate_point10(rotated_point, currentBoneIndex,  currentBoneTransformSpaceMatrix);

    // теперь важное разветвление
//     if(currentBoneIndex == 0)   // если кость - сразу рутовая (нулевая), то надо сразу перейти обратно в мир, не входя в цикл, коего нет
//     {
//         currentBoneTransformSpaceMatrix  = int(TRANSFORMNEXT[bone_index][0][0]);    //индекс-указатель на матрицу перевода в пространство кости currentBoneIndex
//         rotated_point = rotate_point10(rotated_point, currentBoneIndex,  currentBoneTransformSpaceMatrix);
//     }
//     else                        // а если есть родительские кости, то пройдемся циклом по ним
    {
        for(int i=1; i<16; i++)
        {
            int c = i/4;
            int s = i%4;

            int currentBoneIndex = int(IERARCHY[bone_index][c][s]); // циклом for перебираем кости ветви IERARCHY[bone_index]

            //индекс-указатель на матрицу перевода в пространство кости currentBoneIndex
            int currentBoneTransformSpaceMatrix  = int(TRANSFORMNEXT[bone_index][c][s]);    //индекс-указатель на матрицу перевода в пространство кости currentBoneIndex

            // а далее, при переборе всей ветви, проворачиваем уже по родительским костям и в качестве переходных им указываем матрицы ВЫХОДНЫЕ (TRANSFORMNEXT)
            if(currentBoneIndex != 0) //пока есть кости в ветке (пока индексы ненулевые)
            {
                rotated_point = rotate_point10(rotated_point, currentBoneIndex-1,  currentBoneTransformSpaceMatrix);
            }
            else //как только дошли до конца
            {
                rotated_point = rotate_point10(rotated_point, currentBoneIndex-1,  currentBoneTransformSpaceMatrix);
                break;
            }
        }
    }
    return rotated_point;
}




vec4 update_normal_quat(vec4 quaternion, int bone_index, int transformMatrixIndex)
{
    //чтобы добавить кватернион поворота к кватерниону нормали, сначала домножить на него ЧИСТУЮ матрицу ЛСК кости

    mat4 matrix_clear = mat4(0.0);
        matrix_clear[0] = vec4(LSK_Matrices[bone_index][0].xyz, 0); //LSK-матрица в этом массиве инвертированная, не забывать""
        matrix_clear[1] = vec4(LSK_Matrices[bone_index][1].xyz, 0);
        matrix_clear[2] = vec4(LSK_Matrices[bone_index][2].xyz, 0);
        matrix_clear[3][3] = 1.0f;

    quaternion = CmulCXYZW( inverse(matrix_clear) * POSES[bone_index][0], quaternion );	//поворачиваем вокруг кости // xyzw

    //ВНИМАНИЕ!! Присутствует инверсия ЛСК-матрицы!!

    return quaternion;
}

vec4 calculateBranchRotationNormal(int bone_index, vec4 normal)
{

    vec4 quaternion = vec4(0,0,0,1); //QN_start

	for(int i=0; i<16; i++)
    {
        //пройдемся по матрице, хранящей индексы костей ветви, идущей от кости bone_index
        //и по параллельной матрице, хранящей номера матриц переходов в пространства костей
        //идти будем ВПЛОТЬ до тех пор, пока не дойдем до нулевого индекса. Индексы костей в иерархии увеличены на 1, поэтому нулевой кости не существует в принципе, это позволяет
        //идентифицировать конец ветки
        int c = i/4;
        int s = i%4;

        //int currentBoneIndex                    = int(texelFetch( SKELETON, ivec2(bone_index*16 + i, 0) , 0) );     //текущая кость ветви
        int currentBoneIndex = int(IERARCHY[bone_index][c][s]); //текущая кость ветви

        //int currentBoneTransformSpaceMatrix     = int(texelFetch( SKELETON, ivec2(bone_index*16 + i, 1) , 0) );     //индекс-указатель на матрицу перевода в пространство кости bone_index
        int currentBoneTransformSpaceMatrix     = int(TRANSFORMINTO[bone_index][c][s]);    //индекс-указатель на матрицу перевода в пространство кости bone_index

        if(currentBoneIndex != 0) //пока есть кости в ветке (пока индексы ненулевые)
        {
            quaternion = update_normal_quat(quaternion, currentBoneIndex-1,  currentBoneTransformSpaceMatrix);
            //l = i;  //для корректного преобразования обратно запомним последюю точку ветки иерархии
        }
        else //как только дошли до конца
        {
            break;
        }
    }

    //к этому моменту скопили полный кватернион поворота нормали
    normal = RotByQuatXYZW_3(vec4(normal.xyz, 1), quaternion);    //и повернем нормаль в итоговом направлении

    return normal;
}



void main()
{
    vec4 result = vec4(0);  //вектор для хранения анимированной координаты точки
    vec4 result_normal = vec4(0,0,1,0); //вектор для хранения анимированной нормали


    if(skeletal == 1)
    {
        //всего 7 влияющих костей
        int fb_0 			 = int(B1B2.x);		//0-я кость будет всегда
        float fb_0_w 	     = float(B1B2.y);   //у этой кости всегда будет вес - либо число, лиюо 0, т.к.в модели при отсутствии костей прописываются 8 нулей
        int fb_1 		     = int(B1B2.z);
        float fb_1_w 	     = float(B1B2.w);

        int fb_2 			 = int(B3B4.x);
        float fb_2_w 	     = float(B3B4.y);
        int fb_3 			 = int(B3B4.z);
        float fb_3_w 	     = float(B3B4.w);

        int fb_4 			 = int(B5B6.x);
        float fb_4_w 	     = float(B5B6.y);
        int fb_5 			 = int(B5B6.z);
        float fb_5_w 	     = float(B5B6.w);

        int fb_6 			 = int(B7.x);
        float fb_6_w 	     = float(B7.y);


        vec4 rotated_point_0 = vec4(aPos, 1.0f);
        vec4 rotated_point_1 = vec4(aPos, 1.0f);
        vec4 rotated_point_2 = vec4(aPos, 1.0f);
        vec4 rotated_point_3 = vec4(aPos, 1.0f);
        vec4 rotated_point_4 = vec4(aPos, 1.0f);
        vec4 rotated_point_5 = vec4(aPos, 1.0f);
        vec4 rotated_point_6 = vec4(aPos, 1.0f);

        vec4 rotated_normal_0 = vec4(aNormal, 1.0f);
        vec4 rotated_normal_1 = vec4(aNormal, 1.0f);
        vec4 rotated_normal_2 = vec4(aNormal, 1.0f);
        vec4 rotated_normal_3 = vec4(aNormal, 1.0f);
        vec4 rotated_normal_4 = vec4(aNormal, 1.0f);
        vec4 rotated_normal_5 = vec4(aNormal, 1.0f);
        vec4 rotated_normal_6 = vec4(aNormal, 1.0f);

        float porog = 0.001;
        vec4 chislitel = vec4(0);
        float znamenatel = 0.0;

        //ПЕРВАЯ ВЛИЯЮЩАЯ КОСТЬ
        if(fb_0_w > porog) {rotated_point_0 = calculateBranchRotation11(fb_0, rotated_point_0 ); chislitel+=rotated_point_0*fb_0_w; znamenatel+=fb_0_w;}
        //ВТОРАЯ ВЛИЯЮЩАЯ КОСТЬ
        if(fb_1_w > porog) {rotated_point_1 = calculateBranchRotation11(fb_1, rotated_point_1 ); chislitel+=rotated_point_1*fb_1_w; znamenatel+=fb_1_w;}
        //ТРЕТЬЯ ВЛИЯЮЩАЯ КОСТЬ
        if(fb_2_w > porog) {rotated_point_2 = calculateBranchRotation11(fb_2, rotated_point_2 ); chislitel+=rotated_point_2*fb_2_w; znamenatel+=fb_2_w;}
        //ЧЕТВЕРТАЯ ВЛИЯЮЩАЯ КОСТЬ
        if(fb_3_w > porog) {rotated_point_3 = calculateBranchRotation11(fb_3, rotated_point_3 ); chislitel+=rotated_point_3*fb_3_w; znamenatel+=fb_3_w;}

        if(fb_4_w > porog) {rotated_point_4 = calculateBranchRotation11(fb_4, rotated_point_4 ); chislitel+=rotated_point_4*fb_4_w; znamenatel+=fb_4_w;}

        if(fb_5_w > porog) {rotated_point_5 = calculateBranchRotation11(fb_5, rotated_point_5 ); chislitel+=rotated_point_5*fb_5_w; znamenatel+=fb_5_w;}

        if(fb_6_w > porog) {rotated_point_6 = calculateBranchRotation11(fb_6, rotated_point_6 ); chislitel+=rotated_point_6*fb_6_w; znamenatel+=fb_6_w;}

        result = chislitel/znamenatel; //СРЕДНЕЕ ВЗВЕШЕННОЕ ПОЛОЖЕНИЯ

    //       vec4 result = (rotated_point_0 * fb_0_w + rotated_point_1 * fb_1_w + rotated_point_2*fb_2_w + rotated_point_3*fb_3_w + rotated_point_4*fb_4_w + rotated_point_5*fb_5_w + rotated_point_6*fb_6_w)  / (fb_0_w + fb_1_w + fb_2_w + fb_3_w + fb_4_w + fb_5_w + fb_6_w);//СРЕДНЕЕ ВЗВЕШЕННОЕ ПОЛОЖЕНИЯ

        //практика показывает, что для достоверного рещзультата достаточно и 4 костей
        //vec4 result = (rotated_point_0 * fb_0_w + rotated_point_1 * fb_1_w + rotated_point_2*fb_2_w + rotated_point_3*fb_3_w)  / (fb_0_w + fb_1_w + fb_2_w + fb_3_w);//СРЕДНЕЕ ВЗВЕШЕННОЕ ПОЛОЖЕНИЯ

        vec4 chislitel_normali = vec4(0);
        float znamenatel_normali = 0.0;

        //ПЕРВАЯ ВЛИЯЮЩАЯ КОСТЬ
        if(fb_0_w > porog) {rotated_normal_0 = calculateBranchRotationNormal(fb_0, rotated_normal_0 ); chislitel_normali+=rotated_normal_0*fb_0_w; znamenatel_normali+=fb_0_w;}
        //ВТОРАЯ ВЛИЯЮЩАЯ КОСТЬ
        if(fb_1_w > porog) {rotated_normal_1 = calculateBranchRotationNormal(fb_1, rotated_normal_1 ); chislitel_normali+=rotated_normal_1*fb_1_w; znamenatel_normali+=fb_1_w;}
    	 //ТРЕТЬЯ ВЛИЯЮЩАЯ КОСТЬ
        if(fb_2_w > porog) {rotated_normal_2 = calculateBranchRotationNormal(fb_2, rotated_normal_2 ); chislitel_normali+=rotated_normal_2*fb_2_w; znamenatel_normali+=fb_2_w;}
        //ЧЕТВЕРТАЯ ВЛИЯЮЩАЯ КОСТЬ
        if(fb_3_w > porog) {rotated_normal_3 = calculateBranchRotationNormal(fb_3, rotated_normal_3 ); chislitel_normali+=rotated_normal_3*fb_3_w; znamenatel_normali+=fb_3_w;}

        if(fb_4_w > porog) {rotated_normal_4 = calculateBranchRotationNormal(fb_4, rotated_normal_4 ); chislitel_normali+=rotated_normal_4*fb_4_w; znamenatel_normali+=fb_4_w;}

        if(fb_5_w > porog) {rotated_normal_5 = calculateBranchRotationNormal(fb_5, rotated_normal_5 ); chislitel_normali+=rotated_normal_5*fb_5_w; znamenatel_normali+=fb_5_w;}

        if(fb_6_w > porog) {rotated_normal_6 = calculateBranchRotationNormal(fb_6, rotated_normal_6 ); chislitel_normali+=rotated_normal_6*fb_6_w; znamenatel_normali+=fb_6_w;}

    //     vec4 result_normal = (rotated_normal_0 * fb_0_w + rotated_normal_1 * fb_1_w + rotated_normal_2*fb_2_w + rotated_normal_3*fb_3_w + rotated_normal_4*fb_4_w + rotated_normal_5*fb_5_w + 0.000) / (fb_0_w + fb_1_w + fb_2_w + fb_3_w + fb_4_w + fb_5_w + 0.000);//СРЕДНЕЕ ВЗВЕШЕННОЕ



        result_normal = chislitel_normali/znamenatel_normali; // (rotated_normal_0 * fb_0_w + rotated_normal_1 * fb_1_w ) / (fb_0_w + fb_1_w );//СРЕДНЕЕ ВЗВЕШЕННОЕ


    }


    gl_Position = skeletal == 1 ?  PV*M*result :  PV*M*vec4(aPos,1.0f); //позиция вершины после поворота и перемещения


    vs_out.ourColor = skeletal == 1 ? LSK_Matrices[30][1].xyz : vec3(0, 0.8, 0) ;
    vs_out.TexCoord = aTexCoord;
    vs_out.matIndex = aIndex;
	vs_out.Pos = skeletal == 1 ? M*result : M*vec4(aPos,1.0f);
    vs_out.Normal = skeletal == 1 ?  result_normal : vec4(aNormal,1.0f);//;
    vs_out.txrxRotationMatrix = mat3(transpose(inverse(M)));
    vs_out.CameraPos = CameraPosition.xyz;
    vs_out.CameraDir = CameraDirection.xyz;
	
}
