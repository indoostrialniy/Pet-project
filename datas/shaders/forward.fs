#version 440 core
layout (location = 0) out vec4 FragColor;
// out vec4 FragColor;

in VS_OUT
{
    vec3 ourColor;
    vec2 TexCoord;
    float matIndex;
    vec4 Pos;
    vec4 Normal;
    mat3 txrxRotationMatrix;
    vec3 CameraPos;
    vec3 CameraDir;

} fs_in;

layout(binding = 10, std140) uniform DYNAMIC_LIGHTS_UBO
{
    mat4 LIGHTS[20]; //10 матриц, описывающих 20 источников света

    //LIGHTS[0] - Position;
    //LIGHTS[1] - Direction;
    //LIGHTS[2] - Color;
    //LIGHTS[3] - TypeSectorRadiusPower;   // x - тип (omni, spot, etc), y-ширина конуса луча, если тип=spot,  z- радиус освещения (для оптимизации), w-мощность
};

uniform sampler2DArray material_20; //layer 0 = texture, layer 1 = normal, layer 2 = roughness,
uniform sampler2DArray material_21;
uniform sampler2DArray material_22;
uniform sampler2DArray material_23;
uniform sampler2DArray material_24;
uniform sampler2DArray material_25;
uniform sampler2DArray material_26;
uniform sampler2DArray material_27;

uniform mat4 material_20_Configs;
uniform mat4 material_21_Configs;
uniform mat4 material_22_Configs;
uniform mat4 material_23_Configs;
uniform mat4 material_24_Configs;
uniform mat4 material_25_Configs;
uniform mat4 material_26_Configs;
uniform mat4 material_27_Configs;

uniform int hasLightmap;
uniform sampler2D LIGHTMAP_IMAGE;

// mat4 matInfo = mat4(color,normal,rough,metal
//                     spec,lightmap,reserv1,reserv2
//                     color_sc,normal_sc,rough_sc,metal_sc
//                     spec_sc,lightmap_sc,reserv1_sc,reserv2_sc );

vec4 lightSourceColor = vec4(1, 0.998, 0.678,0);

vec3 Fonar = vec3(0,2,2);
vec3 FonarDir = vec3(0,0,-1);



vec3 logAdd(vec3 u, vec3 v) //вернем логарифмическую сумму векторов
{
    float x = 10*log( pow(10,0.1*u.x) + pow(10,0.1*u.x) );
    float y = 10*log( pow(10,0.1*u.y) + pow(10,0.1*u.y) );
    float z = 10*log( pow(10,0.1*u.z) + pow(10,0.1*u.z) );

    return vec3(x,y,z);
}

float kgamma = 1.5;

vec4 gamma_correction(vec4 color)
{
    float x = pow(color.x, 1/kgamma);
    float y = pow(color.y, 1/kgamma);
    float z = pow(color.z, 1/kgamma);
    float w = pow(color.w, 1/kgamma);
    return vec4(x,y,z,w);
}

float power = 2.4;

vec4 calculateLightFromSourceByPhong(vec4 FragColor, int index, vec3 lightPos)//, float power, vec3 lightColor, vec3 lightSourceDir)
{
    //vec3 lightPos = vec3(-6,4,2);//LIGHTS[index][0].xyz; //мировые координаты источника света
    //float power = 10;
    vec3 lightColor = vec3(1);//LIGHTS[index][2].xyz;
    vec3 lightSourceDir = vec3(0,0,-1);//LIGHTS[index][1].xyz;

    float lengthToLight = length(lightPos - fs_in.Pos.xyz); //расстояние до источника света

    float lightVolumeRadius = 10; //радиус влияния источника света

    float Kc = 1.00; //для истанции 32 метра
    float Kl = 4.5/lightVolumeRadius;
    float Kq = pow(9/lightVolumeRadius,2);
    float fatt = 1 / ( Kc + Kl*lengthToLight + Kq*lengthToLight*lengthToLight );

    if(lengthToLight <= lightVolumeRadius)	//если фрагмент в радиусе действия источника
    {
        vec3 lightDir = normalize(lightPos - fs_in.Pos.xyz);    //вектор на источник света
        vec3 N = normalize(fs_in.txrxRotationMatrix * fs_in.Normal.xyz);//нормаль фрагмента

        //DIFFUSE
        vec3    kd = FragColor.xyz;            //k_diffuse коэффициенты диффузного отражения - цвета из текстуры материала
        float   LN = max(dot(lightDir,N),0.0); //скаляр для учета угла падения луча
        vec3    i_light = power*lightColor*fatt;///pow(lengthToLight,2);//*power;//*1/3.14;//интенсивность излучения источника, образуется из цвета и удаленности
        vec3 diffuse = kd * i_light * LN;

        //SPECULAR
        vec3    ks = vec3(0.1);//FragColor.xyz;            //k_spec коэффициенты зеркального отражения - цвета из текстуры материала
        //i_light
        vec3 reflectDir = reflect(-lightDir, N); //отраженный от фрагмента луч света
        float shininess = 22; //шершавость?
        vec3 camera = normalize(fs_in.Pos.xyz -fs_in.CameraPos); //вектор к зрителю
        float spec = pow(max(dot(-camera, reflectDir), 0.0), shininess);
        vec3 specular = ks * i_light * spec;

        //AMBIENT
        float ia = 0.01; // интенсивность окружающего освещения
        vec3 ka = ia* FragColor.xyz; // материал окружающего освещения
        vec3 ambient = ka*i_light;


        float spot = dot(lightSourceDir, -lightDir) > 0.8 ? 1 : 0;
        //spot = smoothstep(0.55, 0.95, dot(lightSourceDir, -lightDir) );    //определим критерии отсечения конуса света фонарика

        return vec4(ambient*0 + diffuse*1 + specular*0, 1.0) * spot;
    }
//     else
//     {
//         return vec4(1,1,0,1);
//     }
}

vec4 calculateLightFromSourceByPBR(vec4 FragColor, int index, vec3 lightPos)//, float power, vec3 lightColor, vec3 lightSourceDir)
{
    //vec3 lightPos = vec3(0,0,1.5);//LIGHTS[index][0].xyz; //мировые координаты источника света

    vec3 lightColor = LIGHTS[index][2].xyz;
    vec3 lightSourceDir = LIGHTS[index][1].xyz; //vec3(0,0,-1);//

    float lengthToLight = length(lightPos - fs_in.Pos.xyz); //расстояние до источника света

    float lightVolumeRadius = 32; //радиус влияния источника света

    float Kc = 1.00; //для истанции 32 метра
    float Kl = 4.5/lightVolumeRadius;
    float Kq = pow(9/lightVolumeRadius,2);
    float fatt = 1 / ( Kc + Kl*lengthToLight + Kq*lengthToLight*lengthToLight );

    vec3 lightDir = normalize(lightPos - fs_in.Pos.xyz);    //вектор на источник света
    vec3 N = normalize(fs_in.txrxRotationMatrix * fs_in.Normal.xyz);//нормаль фрагмента

    //if(lengthToLight <= lightVolumeRadius)	//если фрагмент в радиусе действия источника
    {
        vec3 L_i = power*lightColor*fatt;///(pow(lengthToLight,2));// * fatt;//*power;///3.14; //количество света от источника с угасанием
        float wn = max(dot(lightDir, N), 0.0); //закон косинуса Ламберта

        vec3 f_Lambert = FragColor.xyz;///3.14;  //функция Ламберта

        //kd + ks = 1
        float kd = 0.99; //типа все рассеивается?

        vec3 fd = kd * f_Lambert;


        //vec3 fs = D*F*G/(4*wi*n*w0*n);

        vec3 fr = fd;//+fs;// BRDF


        vec3 L_e = 0.1 * FragColor.xyz; //L emitted, излучаемый поверхностью свет, аналогия с ambient

        vec3 L_r = fr * L_i * wn;// * 3.14;   //L reflected

        float spot = dot(lightSourceDir, -lightDir) > 0.9 ? 1 : 0;
        spot = smoothstep(LIGHTS[index][3].y, 0.9, dot(lightSourceDir, -lightDir) ); //в smoothstep второе число определяет четкую внутреннюю грань, а второе - размытие вне круга

        vec3 L_out = LIGHTS[index][3].x < 0.5f ? L_r : L_r*spot ;

        //L_out = L_r;
        return vec4(L_out, 1.0);
    }
//     else
//     {
//         return vec4(1,1,0,1);
//     }
}

vec3 calculateColor()
{
    vec3 lightPos = LIGHTS[0][0].xyz; //мировые координаты источника света
    vec3 N = normalize(fs_in.txrxRotationMatrix * fs_in.Normal.xyz);//нормаль фрагмента
    vec3 camera = normalize(fs_in.Pos.xyz - fs_in.CameraPos); //вектор к зрителю

    float lengthToLight = length(fs_in.Pos.xyz - lightPos)/10;

    float lightVolumeRadius = 10; //радиус влияния источника света

    float Kc = 1.00; //для истанции 32 метра
    float Kl = 4.5/lightVolumeRadius;
    float Kq = pow(9/lightVolumeRadius,2);
    float fatt = 1 / ( Kc + Kl*lengthToLight + Kq*lengthToLight*lengthToLight );

    vec3 Length = vec3(1-lengthToLight);

    return Length;//reflect(camera, N);//vec3(1);//;
}



void main()
{
    vec4 diffuseColor = vec4(0);
    vec4 lightmapColor = vec4(0);

    if(fs_in.matIndex < 0.9)
    {
        //адреса инкремментированы на 1, так что надо это скомпенсировать
        int colorAdress 	= int(material_20_Configs[0][0]);
        int colorScale 		= int(material_20_Configs[2][0]);
        int normalAdress    = int(material_20_Configs[0][1]);

        diffuseColor = texture(material_20, vec3( colorScale*fs_in.TexCoord, colorAdress-1));   //определили собственный цвет самой текстуры поверхности
    }

    else if(fs_in.matIndex < 1.9 && fs_in.matIndex > 0.9)
    {
        int colorAdress = int(material_21_Configs[0][0]);
        int colorScale = int(material_21_Configs[2][0]);
        diffuseColor = texture(material_21, vec3( colorScale*fs_in.TexCoord, colorAdress-1));
    }


    else if(fs_in.matIndex < 2.9 && fs_in.matIndex > 1.9)
    {
        int colorAdress = int(material_22_Configs[0][0]);
        int colorScale = int(material_22_Configs[2][0]);
        diffuseColor = texture(material_22, vec3( colorScale*fs_in.TexCoord, colorAdress-1));
    }
    float alpha = diffuseColor.w;

    FragColor = diffuseColor;//*0.15;//

    if(alpha < 0.2f) //если альфа черный, значит прозрачный
    {
        //FragColor = texture(material_20, vec3( fs_in.TexCoord, 0));

        discard;

    }


    // 1
    //FragColor = vec4(diffuseColor.xyz,1);   //тупо цвет текстуры

    // 2
    FragColor = vec4(diffuseColor.xyz,1) * 0.05;   //почти полная темнота диффузии

    // 3 добавили динамические источники, квадратный корень суммы квадратов
	for (int l=0; l<10; l++)
	{
        vec4 temp = calculateLightFromSourceByPBR(diffuseColor, l, LIGHTS[l][0].xyz);
        temp = vec4( pow(temp.x,2), pow(temp.y,2), pow(temp.z,2), pow(temp.w,2) );
        FragColor += temp;//calculateLightFromSourceByPBR(diffuseColor, l, LIGHTS[l][0].xyz);//, LIGHT[l][1].xyz);
	}
	FragColor = vec4( sqrt(FragColor.x), sqrt(FragColor.y), sqrt(FragColor.z), sqrt(FragColor.z) );


}
