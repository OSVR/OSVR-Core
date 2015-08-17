// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#version 430 core

in INFO {
    vec2 texCoords; //Tex coords for a given gl_Vertex
    float k1_red;
    float k1_green;
    float k1_blue;
    vec2 center;
} fs_in;

out vec4 color;

layout (binding = 0) uniform sampler2D s;

vec2 Distort(vec2 p, float k1)
{
    float r2 = p.x * p.x + p.y * p.y;
    float newRadius = (1 + k1*r2);
    p.x = p.x * newRadius;
    p.y = p.y * newRadius;

    return p;
}

void main()
{
    vec2 uv_red, uv_green, uv_blue;
    vec4 color_red, color_green, color_blue;
    vec2 sectorOrigin;

    // Radial distort around center
    sectorOrigin = fs_in.center.xy;

    uv_red      =  Distort(fs_in.texCoords-sectorOrigin, fs_in.k1_red)      + sectorOrigin;
    uv_green    =  Distort(fs_in.texCoords-sectorOrigin, fs_in.k1_green)    + sectorOrigin;
    uv_blue     =  Distort(fs_in.texCoords-sectorOrigin, fs_in.k1_blue)     + sectorOrigin;

    color_red   = texture2D(s, uv_red   );
    color_green = texture2D(s, uv_green );
    color_blue  = texture2D(s, uv_blue  );

    if (    ((uv_red.x>0)     && (uv_red.x<1)        && (uv_red.y>0)     && (uv_red.y<1)))
    {
        color = vec4(color_red.x, color_green.y, color_blue.z, 1.0);
    } else {
        color = vec4(0,0,0,0); //black
    }
}
