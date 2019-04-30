/* Goxel 3D voxels editor
 *
 * copyright (c) 2018 Guillaume Chereau <guillaume@noctua-software.com>
 *
 * Goxel is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.

 * Goxel is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.

 * You should have received a copy of the GNU General Public License along with
 * goxel.  If not, see <http://www.gnu.org/licenses/>.
 */

/* This file is autogenerated by tools/create_assets.py */

{.path = "data/shaders/background.glsl", .size = 570, .data =
    "varying lowp vec4 v_color;\n"
    "\n"
    "#ifdef VERTEX_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "attribute highp vec3 a_pos;\n"
    "attribute lowp  vec4 a_color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(a_pos, 1.0);\n"
    "    v_color = a_color;\n"
    "}\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    "\n"
    "#ifdef FRAGMENT_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = v_color;\n"
    "}\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    ""
},
{.path = "data/shaders/mesh.glsl", .size = 4258, .data =
    "/*\n"
    " * I followed those name conventions.  All the vectors are expressed in eye\n"
    " * coordinates.\n"
    " *\n"
    " *         reflection         light source\n"
    " *\n"
    " *               r              s\n"
    " *                 ^         ^\n"
    " *                  \\   n   /\n"
    " *  eye              \\  ^  /\n"
    " *     v  <....       \\ | /\n"
    " *              -----__\\|/\n"
    " *                  ----+----\n"
    " *\n"
    " *\n"
    " */\n"
    "\n"
    "uniform highp mat4  u_model;\n"
    "uniform highp mat4  u_view;\n"
    "uniform highp mat4  u_proj;\n"
    "uniform highp mat4  u_shadow_mvp;\n"
    "uniform lowp  float u_pos_scale;\n"
    "\n"
    "// Light parameters\n"
    "uniform lowp    vec3  u_l_dir;\n"
    "uniform lowp    float u_l_int;\n"
    "\n"
    "// Material parameters\n"
    "uniform lowp float u_m_amb; // Ambient light coef.\n"
    "uniform lowp float u_m_dif; // Diffuse light coef.\n"
    "uniform lowp float u_m_spe; // Specular light coef.\n"
    "uniform lowp float u_m_shi; // Specular light shininess.\n"
    "uniform lowp float u_m_smo; // Smoothness.\n"
    "\n"
    "uniform mediump sampler2D u_occlusion_tex;\n"
    "uniform mediump sampler2D u_bump_tex;\n"
    "uniform mediump float     u_occlusion;\n"
    "uniform mediump sampler2D u_shadow_tex;\n"
    "uniform mediump float     u_shadow_k;\n"
    "\n"
    "varying highp   vec3 v_pos;\n"
    "varying lowp    vec4 v_color;\n"
    "varying mediump vec2 v_occlusion_uv;\n"
    "varying mediump vec2 v_uv;\n"
    "varying mediump vec2 v_bump_uv;\n"
    "varying mediump vec3 v_normal;\n"
    "varying mediump vec4 v_shadow_coord;\n"
    "\n"
    "#ifdef VERTEX_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "attribute highp   vec3 a_pos;\n"
    "attribute mediump vec3 a_normal;\n"
    "attribute lowp    vec4 a_color;\n"
    "attribute mediump vec2 a_occlusion_uv;\n"
    "attribute mediump vec2 a_bump_uv;   // bump tex base coordinates [0,255]\n"
    "attribute mediump vec2 a_uv;        // uv coordinates [0,1]\n"
    "\n"
    "void main()\n"
    "{\n"
    "    v_normal = a_normal;\n"
    "    v_color = a_color;\n"
    "    v_occlusion_uv = (a_occlusion_uv + 0.5) / (16.0 * VOXEL_TEXTURE_SIZE);\n"
    "    v_pos = a_pos * u_pos_scale;\n"
    "    v_uv = a_uv;\n"
    "    v_bump_uv = a_bump_uv;\n"
    "    gl_Position = u_proj * u_view * u_model * vec4(v_pos, 1.0);\n"
    "    v_shadow_coord = (u_shadow_mvp * u_model * vec4(v_pos, 1.0));\n"
    "}\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    "\n"
    "#ifdef FRAGMENT_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "mediump vec2 uv, bump_uv;\n"
    "mediump vec3 n, s, r, v, bump;\n"
    "mediump float s_dot_n;\n"
    "lowp float l_amb, l_dif, l_spe;\n"
    "lowp float occlusion;\n"
    "lowp float visibility;\n"
    "lowp vec2 PS[4]; // Poisson offsets used for the shadow map.\n"
    "int i;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    // clamp uv so to prevent overflow with multismapling.\n"
    "    uv = clamp(v_uv, 0.0, 1.0);\n"
    "    s = u_l_dir;\n"
    "    n = normalize((u_view * u_model * vec4(v_normal, 0.0)).xyz);\n"
    "    bump_uv = (v_bump_uv + 0.5 + uv * 15.0) / 256.0;\n"
    "    bump = texture2D(u_bump_tex, bump_uv).xyz - 0.5;\n"
    "    bump = normalize((u_view * u_model * vec4(bump, 0.0)).xyz);\n"
    "    n = mix(bump, n, u_m_smo);\n"
    "    s_dot_n = dot(s, n);\n"
    "    l_dif = u_m_dif * max(0.0, s_dot_n);\n"
    "    l_amb = u_m_amb;\n"
    "\n"
    "    // Specular light.\n"
    "    v = normalize(-(u_view * u_model * vec4(v_pos, 1.0)).xyz);\n"
    "    r = reflect(-s, n);\n"
    "    l_spe = u_m_spe * pow(max(dot(r, v), 0.0), u_m_shi);\n"
    "    l_spe = s_dot_n > 0.0 ? l_spe : 0.0;\n"
    "\n"
    "    occlusion = texture2D(u_occlusion_tex, v_occlusion_uv).r;\n"
    "    occlusion = sqrt(occlusion);\n"
    "    occlusion = mix(1.0, occlusion, u_occlusion);\n"
    "    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "    gl_FragColor.rgb += (l_dif + l_amb) * u_l_int * v_color.rgb;\n"
    "    gl_FragColor.rgb += l_spe * u_l_int * vec3(1.0);\n"
    "    gl_FragColor.rgb *= occlusion;\n"
    "\n"
    "    // Shadow map.\n"
    "    #ifdef SHADOW\n"
    "    visibility = 1.0;\n"
    "    mediump vec4 shadow_coord = v_shadow_coord / v_shadow_coord.w;\n"
    "    lowp float bias = 0.005 * tan(acos(clamp(s_dot_n, 0.0, 1.0)));\n"
    "    bias = clamp(bias, 0.0, 0.015);\n"
    "    shadow_coord.z -= bias;\n"
    "    PS[0] = vec2(-0.94201624, -0.39906216) / 1024.0;\n"
    "    PS[1] = vec2(+0.94558609, -0.76890725) / 1024.0;\n"
    "    PS[2] = vec2(-0.09418410, -0.92938870) / 1024.0;\n"
    "    PS[3] = vec2(+0.34495938, +0.29387760) / 1024.0;\n"
    "    for (i = 0; i < 4; i++)\n"
    "        if (texture2D(u_shadow_tex, v_shadow_coord.xy +\n"
    "           PS[i]).z < shadow_coord.z) visibility -= 0.2;\n"
    "    if (s_dot_n <= 0.0) visibility = 0.5;\n"
    "    gl_FragColor.rgb *= mix(1.0, visibility, u_shadow_k);\n"
    "    #endif\n"
    "\n"
    "}\n"
    "\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    ""
},
{.path = "data/shaders/model3d.glsl", .size = 2766, .data =
    "#if defined(GL_ES) && defined(FRAGMENT_SHADER)\n"
    "#extension GL_OES_standard_derivatives : enable\n"
    "#endif\n"
    "\n"
    "uniform highp   mat4  u_model;\n"
    "uniform highp   mat4  u_view;\n"
    "uniform highp   mat4  u_proj;\n"
    "uniform highp   mat4  u_clip;\n"
    "uniform lowp    vec4  u_color;\n"
    "uniform mediump vec2  u_uv_scale;\n"
    "uniform lowp    float u_grid_alpha;\n"
    "uniform mediump vec3  u_l_dir;\n"
    "uniform mediump float u_l_diff;\n"
    "uniform mediump float u_l_emit;\n"
    "\n"
    "uniform mediump sampler2D u_tex;\n"
    "uniform mediump float     u_strip;\n"
    "uniform mediump float     u_time;\n"
    "\n"
    "varying mediump vec3 v_normal;\n"
    "varying highp   vec3 v_pos;\n"
    "varying lowp    vec4 v_color;\n"
    "varying mediump vec2 v_uv;\n"
    "varying mediump vec4 v_clip_pos;\n"
    "\n"
    "#ifdef VERTEX_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "attribute highp   vec3  a_pos;\n"
    "attribute lowp    vec4  a_color;\n"
    "attribute mediump vec3  a_normal;\n"
    "attribute mediump vec2  a_uv;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    lowp vec4 col = u_color * a_color;\n"
    "    highp vec3 pos = (u_view * u_model * vec4(a_pos, 1.0)).xyz;\n"
    "    if (u_clip[3][3] > 0.0)\n"
    "        v_clip_pos = u_clip * u_model * vec4(a_pos, 1.0);\n"
    "    gl_Position = u_proj * vec4(pos, 1.0);\n"
    "    mediump float diff = max(0.0, dot(u_l_dir, a_normal));\n"
    "    col.rgb *= (u_l_emit + u_l_diff * diff);\n"
    "    v_color = col;\n"
    "    v_uv = a_uv * u_uv_scale;\n"
    "    v_pos = (u_model * vec4(a_pos, 1.0)).xyz;\n"
    "    v_normal = a_normal;\n"
    "}\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    "\n"
    "#ifdef FRAGMENT_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = v_color * texture2D(u_tex, v_uv);\n"
    "    if (u_strip > 0.0) {\n"
    "       mediump float p = gl_FragCoord.x + gl_FragCoord.y + u_time * 4.0;\n"
    "       if (mod(p, 8.0) < 4.0) gl_FragColor.rgb *= 0.5;\n"
    "    }\n"
    "    if (u_clip[3][3] > 0.0) {\n"
    "        if (    v_clip_pos[0] < -v_clip_pos[3] ||\n"
    "                v_clip_pos[1] < -v_clip_pos[3] ||\n"
    "                v_clip_pos[2] < -v_clip_pos[3] ||\n"
    "                v_clip_pos[0] > +v_clip_pos[3] ||\n"
    "                v_clip_pos[1] > +v_clip_pos[3] ||\n"
    "                v_clip_pos[2] > +v_clip_pos[3]) discard;\n"
    "    }\n"
    "\n"
    "    // Grid effect.\n"
    "#if !defined(GL_ES) || defined(GL_OES_standard_derivatives)\n"
    "    if (u_grid_alpha > 0.0) {\n"
    "        mediump vec2 c;\n"
    "        if (abs((u_model * vec4(v_normal, 0.0)).x) > 0.5) c = v_pos.yz;\n"
    "        if (abs((u_model * vec4(v_normal, 0.0)).y) > 0.5) c = v_pos.zx;\n"
    "        if (abs((u_model * vec4(v_normal, 0.0)).z) > 0.5) c = v_pos.xy;\n"
    "        mediump vec2 grid = abs(fract(c - 0.5) - 0.5) / fwidth(c);\n"
    "        mediump float line = min(grid.x, grid.y);\n"
    "        gl_FragColor.rgb *= mix(1.0 - u_grid_alpha, 1.0, min(line, 1.0));\n"
    "    }\n"
    "#endif\n"
    "\n"
    "}\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    ""
},
{.path = "data/shaders/pbr.glsl", .size = 11829, .data =
    "\n"
    "// Just a test.  Based on the glTF 2 PBR shader.\n"
    "\n"
    "#define LIGHT_COUNT 1\n"
    "\n"
    "// KHR_lights_punctual extension.\n"
    "// see https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_lights_punctual\n"
    "struct Light\n"
    "{\n"
    "    vec3 direction;\n"
    "    vec3 color;\n"
    "    float intensity;\n"
    "    vec2 padding;\n"
    "};\n"
    "\n"
    "varying vec2 v_UVCoord1;\n"
    "varying vec2 v_UVCoord2;\n"
    "varying vec3 v_Position;\n"
    "varying vec3 v_Color;\n"
    "varying mat3 v_TBN;\n"
    "\n"
    "uniform mat4 u_proj;\n"
    "uniform mat4 u_view;\n"
    "uniform mat4 u_model;\n"
    "uniform mat4 u_normal_matrix = mat4(1.0, 0.0, 0.0, 0.0,\n"
    "                                    0.0, 1.0, 0.0, 0.0,\n"
    "                                    0.0, 0.0, 1.0, 0.0,\n"
    "                                    0.0, 0.0, 0.0, 1.0);\n"
    "\n"
    "uniform float u_MetallicFactor = 0.0;\n"
    "uniform float u_RoughnessFactor = 0.5;\n"
    "uniform vec4 u_BaseColorFactor = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "\n"
    "uniform vec3 u_camera;\n"
    "\n"
    "uniform Light u_Lights[LIGHT_COUNT] = {{\n"
    "    vec3(0.3, 0.1, -1.0),\n"
    "    vec3(1.0, 1.0, 1.0),\n"
    "    2.0,\n"
    "    vec2(0.0, 0.0)\n"
    "}};\n"
    "\n"
    "uniform float u_Exposure = 1.0;\n"
    "uniform float u_Gamma = 2.2;\n"
    "\n"
    "uniform lowp float u_pos_scale = 1.0;\n"
    "\n"
    "uniform sampler2D u_occlusion_tex;\n"
    "uniform int u_OcclusionUVSet = 1;\n"
    "uniform float u_OcclusionStrength = 0.5;\n"
    "\n"
    "uniform sampler2D u_bump_tex;\n"
    "// uniform sampler2D u_NormalSampler;\n"
    "uniform float u_NormalScale = 1.0;\n"
    "uniform sampler2D u_brdf_lut;\n"
    "\n"
    "const float M_PI = 3.141592653589793;\n"
    "\n"
    "\n"
    "#ifdef VERTEX_SHADER\n"
    "\n"
    "attribute vec3 a_pos;\n"
    "attribute vec3 a_normal;\n"
    "attribute vec3 a_tangent;\n"
    "attribute vec4 a_color;\n"
    "attribute vec2 a_occlusion_uv;\n"
    "\n"
    "attribute vec2 a_uv;\n"
    "attribute vec2 a_bump_uv;\n"
    "\n"
    "vec4 getNormal()\n"
    "{\n"
    "    vec4 normal = vec4(a_normal, 0.0);\n"
    "    return normalize(normal);\n"
    "}\n"
    "\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec4 pos = u_model * vec4(a_pos, 1.0) * u_pos_scale;\n"
    "    v_Position = vec3(pos.xyz) / pos.w;\n"
    "\n"
    "    vec4 tangent = vec4(normalize(a_tangent), 1.0);\n"
    "    vec3 normalW = normalize(vec3(u_normal_matrix * vec4(getNormal().xyz, 0.0)));\n"
    "    vec3 tangentW = normalize(vec3(u_model * vec4(tangent.xyz, 0.0)));\n"
    "    vec3 bitangentW = cross(normalW, tangentW) * tangent.w;\n"
    "    v_TBN = mat3(tangentW, bitangentW, normalW);\n"
    "\n"
    "    v_UVCoord1 = (a_bump_uv + 0.5 + a_uv * 15.0) / 256.0;\n"
    "    v_UVCoord2 = (a_occlusion_uv + 0.5) / (16.0 * VOXEL_TEXTURE_SIZE);\n"
    "    v_Color = a_color.rgb;\n"
    "    gl_Position = u_proj * u_view * pos;\n"
    "}\n"
    "\n"
    "#endif\n"
    "\n"
    "#ifdef FRAGMENT_SHADER\n"
    "\n"
    "struct AngularInfo\n"
    "{\n"
    "    float NdotL;                  // cos angle between normal and light direction\n"
    "    float NdotV;                  // cos angle between normal and view direction\n"
    "    float NdotH;                  // cos angle between normal and half vector\n"
    "    float LdotH;                  // cos angle between light direction and half vector\n"
    "\n"
    "    float VdotH;                  // cos angle between view direction and half vector\n"
    "\n"
    "    vec3 padding;\n"
    "};\n"
    "\n"
    "struct MaterialInfo\n"
    "{\n"
    "    float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)\n"
    "    vec3 reflectance0;            // full reflectance color (normal incidence angle)\n"
    "\n"
    "    float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])\n"
    "    vec3 diffuseColor;            // color contribution from diffuse lighting\n"
    "\n"
    "    vec3 reflectance90;           // reflectance color at grazing angle\n"
    "    vec3 specularColor;           // color contribution from specular lighting\n"
    "};\n"
    "\n"
    "\n"
    "vec2 getNormalUV()\n"
    "{\n"
    "    vec3 uv = vec3(v_UVCoord1, 1.0);\n"
    "    return uv.xy;\n"
    "}\n"
    "\n"
    "// Find the normal for this fragment, pulling either from a predefined normal map\n"
    "// or from the interpolated mesh normal and tangent attributes.\n"
    "vec3 getNormal()\n"
    "{\n"
    "    vec2 UV = getNormalUV();\n"
    "    mat3 tbn = v_TBN;\n"
    "    vec3 n = texture2D(u_bump_tex, UV).rgb;\n"
    "    n = normalize(tbn * ((2.0 * n - 1.0) * vec3(u_NormalScale, u_NormalScale, 1.0)));\n"
    "    return n;\n"
    "}\n"
    "\n"
    "vec4 getVertexColor()\n"
    "{\n"
    "    vec4 color = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "    color.rgb = v_Color;\n"
    "    return color;\n"
    "}\n"
    "\n"
    "AngularInfo getAngularInfo(vec3 pointToLight, vec3 normal, vec3 view)\n"
    "{\n"
    "    // Standard one-letter names\n"
    "    vec3 n = normalize(normal);           // Outward direction of surface point\n"
    "    vec3 v = normalize(view);             // Direction from surface point to view\n"
    "    vec3 l = normalize(pointToLight);     // Direction from surface point to light\n"
    "    vec3 h = normalize(l + v);            // Direction of the vector between l and v\n"
    "\n"
    "    float NdotL = clamp(dot(n, l), 0.0, 1.0);\n"
    "    float NdotV = clamp(dot(n, v), 0.0, 1.0);\n"
    "    float NdotH = clamp(dot(n, h), 0.0, 1.0);\n"
    "    float LdotH = clamp(dot(l, h), 0.0, 1.0);\n"
    "    float VdotH = clamp(dot(v, h), 0.0, 1.0);\n"
    "\n"
    "    return AngularInfo(\n"
    "        NdotL,\n"
    "        NdotV,\n"
    "        NdotH,\n"
    "        LdotH,\n"
    "        VdotH,\n"
    "        vec3(0, 0, 0)\n"
    "    );\n"
    "}\n"
    "\n"
    "// The following equation models the Fresnel reflectance term of the spec equation (aka F())\n"
    "// Implementation of fresnel from [4], Equation 15\n"
    "vec3 specularReflection(MaterialInfo materialInfo, AngularInfo angularInfo)\n"
    "{\n"
    "    return materialInfo.reflectance0 + (materialInfo.reflectance90 - materialInfo.reflectance0) * pow(clamp(1.0 - angularInfo.VdotH, 0.0, 1.0), 5.0);\n"
    "}\n"
    "\n"
    "// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())\n"
    "// Implementation from \"Average Irregularity Representation of a Roughened Surface for Ray Reflection\" by T. S. Trowbridge, and K. P. Reitz\n"
    "// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.\n"
    "float microfacetDistribution(MaterialInfo materialInfo, AngularInfo angularInfo)\n"
    "{\n"
    "    float alphaRoughnessSq = materialInfo.alphaRoughness * materialInfo.alphaRoughness;\n"
    "    float f = (angularInfo.NdotH * alphaRoughnessSq - angularInfo.NdotH) * angularInfo.NdotH + 1.0;\n"
    "    return alphaRoughnessSq / (M_PI * f * f);\n"
    "}\n"
    "\n"
    "// Smith Joint GGX\n"
    "// Note: Vis = G / (4 * NdotL * NdotV)\n"
    "// see Eric Heitz. 2014. Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs. Journal of Computer Graphics Techniques, 3\n"
    "// see Real-Time Rendering. Page 331 to 336.\n"
    "// see https://google.github.io/filament/Filament.md.html#materialsystem/specularbrdf/geometricshadowing(specularg)\n"
    "float visibilityOcclusion(MaterialInfo materialInfo, AngularInfo angularInfo)\n"
    "{\n"
    "    float NdotL = angularInfo.NdotL;\n"
    "    float NdotV = angularInfo.NdotV;\n"
    "    float alphaRoughnessSq = materialInfo.alphaRoughness * materialInfo.alphaRoughness;\n"
    "\n"
    "    float GGXV = NdotL * sqrt(NdotV * NdotV * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);\n"
    "    float GGXL = NdotV * sqrt(NdotL * NdotL * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);\n"
    "\n"
    "    return 0.5 / (GGXV + GGXL);\n"
    "}\n"
    "\n"
    "// Lambert lighting\n"
    "// see https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/\n"
    "vec3 diffuse(MaterialInfo materialInfo)\n"
    "{\n"
    "    return materialInfo.diffuseColor / M_PI;\n"
    "}\n"
    "\n"
    "vec3 getPointShade(vec3 pointToLight, MaterialInfo materialInfo, vec3 normal, vec3 view)\n"
    "{\n"
    "    AngularInfo angularInfo = getAngularInfo(pointToLight, normal, view);\n"
    "\n"
    "    // If one of the dot products is larger than zero, no division by zero can happen. Avoids black borders.\n"
    "    if (angularInfo.NdotL > 0.0 || angularInfo.NdotV > 0.0)\n"
    "    {\n"
    "        // Calculate the shading terms for the microfacet specular shading model\n"
    "        vec3 F = specularReflection(materialInfo, angularInfo);\n"
    "        float Vis = visibilityOcclusion(materialInfo, angularInfo);\n"
    "        float D = microfacetDistribution(materialInfo, angularInfo);\n"
    "\n"
    "        // Calculation of analytical lighting contribution\n"
    "        vec3 diffuseContrib = (1.0 - F) * diffuse(materialInfo);\n"
    "        vec3 specContrib = F * Vis * D;\n"
    "\n"
    "        // Obtain final intensity as reflectance (BRDF) scaled by the energy of the light (cosine law)\n"
    "        return angularInfo.NdotL * (diffuseContrib + specContrib);\n"
    "    }\n"
    "\n"
    "    return vec3(0.0, 0.0, 0.0);\n"
    "}\n"
    "\n"
    "vec2 getOcclusionUV()\n"
    "{\n"
    "    vec3 uv = vec3(v_UVCoord1, 1.0);\n"
    "    uv.xy = u_OcclusionUVSet < 1 ? v_UVCoord1 : v_UVCoord2;\n"
    "    return uv.xy;\n"
    "}\n"
    "\n"
    "vec3 applyDirectionalLight(Light light, MaterialInfo materialInfo, vec3 normal, vec3 view)\n"
    "{\n"
    "    vec3 pointToLight = -light.direction;\n"
    "    vec3 shade = getPointShade(pointToLight, materialInfo, normal, view);\n"
    "    return light.intensity * light.color * shade;\n"
    "}\n"
    "\n"
    "// Gamma Correction in Computer Graphics\n"
    "// see https://www.teamten.com/lawrence/graphics/gamma/\n"
    "vec3 gammaCorrection(vec3 color)\n"
    "{\n"
    "    return pow(color, vec3(1.0 / u_Gamma));\n"
    "}\n"
    "\n"
    "vec3 toneMap(vec3 color)\n"
    "{\n"
    "    color *= u_Exposure;\n"
    "    return gammaCorrection(color);\n"
    "}\n"
    "\n"
    "vec3 getIBLContribution(MaterialInfo materialInfo, vec3 n, vec3 v)\n"
    "{\n"
    "    float NdotV = clamp(dot(n, v), 0.0, 1.0);\n"
    "    vec3 reflection = normalize(reflect(-v, n));\n"
    "    vec2 brdfSamplePoint = clamp(vec2(NdotV, materialInfo.perceptualRoughness), vec2(0.0, 0.0), vec2(1.0, 1.0));\n"
    "    // retrieve a scale and bias to F0. See [1], Figure 3\n"
    "    vec2 brdf = texture2D(u_brdf_lut, brdfSamplePoint).rg;\n"
    "\n"
    "    // vec4 diffuseSample = textureCube(u_DiffuseEnvSampler, n);\n"
    "    // vec4 specularSample = textureCube(u_SpecularEnvSampler, reflection);\n"
    "    vec4 diffuseSample = vec4(0.1, 0.1, 0.1, 1.0);\n"
    "    vec4 specularSample = vec4(0.1, 0.1, 0.1, 1.0);\n"
    "\n"
    "    vec3 diffuseLight = diffuseSample.rgb;\n"
    "    vec3 specularLight = specularSample.rgb;\n"
    "    vec3 diffuse = diffuseLight * materialInfo.diffuseColor;\n"
    "    vec3 specular = specularLight * (materialInfo.specularColor * brdf.x + brdf.y);\n"
    "    return diffuse + specular;\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "    // Metallic and Roughness material properties are packed together\n"
    "    // In glTF, these factors can be specified by fixed scalar values\n"
    "    // or from a metallic-roughness map\n"
    "    float perceptualRoughness = 0.0;\n"
    "    float metallic = 0.0;\n"
    "    vec4 baseColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "    vec3 diffuseColor = vec3(0.0);\n"
    "    vec3 specularColor= vec3(0.0);\n"
    "    vec3 f0 = vec3(0.04);\n"
    "\n"
    "    metallic = u_MetallicFactor;\n"
    "    perceptualRoughness = u_RoughnessFactor;\n"
    "\n"
    "    // The albedo may be defined from a base texture or a flat color\n"
    "    baseColor = u_BaseColorFactor;\n"
    "    baseColor *= getVertexColor();\n"
    "    diffuseColor = baseColor.rgb * (vec3(1.0) - f0) * (1.0 - metallic);\n"
    "\n"
    "    specularColor = mix(f0, baseColor.rgb, metallic);\n"
    "\n"
    "    baseColor.a = 1.0;\n"
    "\n"
    "    perceptualRoughness = clamp(perceptualRoughness, 0.0, 1.0);\n"
    "    metallic = clamp(metallic, 0.0, 1.0);\n"
    "\n"
    "    // Roughness is authored as perceptual roughness; as is convention,\n"
    "    // convert to material roughness by squaring the perceptual roughness [2].\n"
    "    float alphaRoughness = perceptualRoughness * perceptualRoughness;\n"
    "\n"
    "    // Compute reflectance.\n"
    "    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);\n"
    "\n"
    "    vec3 specularEnvironmentR0 = specularColor.rgb;\n"
    "    // Anything less than 2% is physically impossible and is instead considered to be shadowing. Compare to \"Real-Time-Rendering\" 4th editon on page 325.\n"
    "    vec3 specularEnvironmentR90 = vec3(clamp(reflectance * 50.0, 0.0, 1.0));\n"
    "\n"
    "    MaterialInfo materialInfo = MaterialInfo(\n"
    "        perceptualRoughness,\n"
    "        specularEnvironmentR0,\n"
    "        alphaRoughness,\n"
    "        diffuseColor,\n"
    "        specularEnvironmentR90,\n"
    "        specularColor\n"
    "    );\n"
    "\n"
    "    // LIGHTING\n"
    "\n"
    "    vec3 color = vec3(0.0, 0.0, 0.0);\n"
    "    vec3 normal = getNormal();\n"
    "    vec3 view = normalize(u_camera - v_Position);\n"
    "\n"
    "    for (int i = 0; i < LIGHT_COUNT; ++i)\n"
    "    {\n"
    "        Light light = u_Lights[i];\n"
    "        color += applyDirectionalLight(light, materialInfo, normal, view);\n"
    "    }\n"
    "\n"
    "    color += getIBLContribution(materialInfo, normal, view);\n"
    "\n"
    "    float ao = 1.0;\n"
    "\n"
    "    // Apply optional PBR terms for additional (optional) shading\n"
    "    ao = texture2D(u_occlusion_tex,  getOcclusionUV()).r;\n"
    "    color = mix(color, color * ao, u_OcclusionStrength);\n"
    "\n"
    "    // regular shading\n"
    "    gl_FragColor = vec4(toneMap(color), baseColor.a);\n"
    "    // gl_FragColor = vec4(color, 1.0);\n"
    "    // gl_FragColor = vec4(diffuseColor, 1.0);\n"
    "\n"
    "}\n"
    "\n"
    "#endif\n"
    ""
},
{.path = "data/shaders/pos_data.glsl", .size = 790, .data =
    "varying lowp  vec2 v_pos_data;\n"
    "uniform highp mat4 u_model;\n"
    "uniform highp mat4 u_view;\n"
    "uniform highp mat4 u_proj;\n"
    "uniform lowp  vec2 u_block_id;\n"
    "\n"
    "#ifdef VERTEX_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "attribute highp vec3 a_pos;\n"
    "attribute lowp  vec2 a_pos_data;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    highp vec3 pos = a_pos;\n"
    "    gl_Position = u_proj * u_view * u_model * vec4(pos, 1.0);\n"
    "    v_pos_data = a_pos_data;\n"
    "}\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    "\n"
    "#ifdef FRAGMENT_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor.rg = u_block_id;\n"
    "    gl_FragColor.ba = v_pos_data;\n"
    "}\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    ""
},
{.path = "data/shaders/shadow_map.glsl", .size = 639, .data =
    "#ifdef VERTEX_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "attribute highp   vec3  a_pos;\n"
    "uniform   highp   mat4  u_model;\n"
    "uniform   highp   mat4  u_view;\n"
    "uniform   highp   mat4  u_proj;\n"
    "uniform   mediump float u_pos_scale;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_proj * u_view * u_model * vec4(a_pos * u_pos_scale, 1.0);\n"
    "}\n"
    "\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    "\n"
    "#ifdef FRAGMENT_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "void main() {}\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    ""
},



