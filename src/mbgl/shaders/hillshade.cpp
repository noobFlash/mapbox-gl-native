// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

  #include <mbgl/shaders/hillshade.hpp>

  namespace mbgl {
  namespace shaders {

  const char* hillshade::name = "hillshade";
  const char* hillshade::vertexSource = R"MBGL_SHADER(
  #ifdef GL_ES
precision highp float;
#else
#define lowp
#define mediump
#define highp
#endif

uniform mat4 u_matrix;

attribute vec2 a_pos;
attribute vec2 a_texture_pos;

varying vec2 v_pos;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
    v_pos = ((a_texture_pos / 8192.0) - 0.5) + 0.5;
}

  )MBGL_SHADER";
  const char* hillshade::fragmentSource = R"MBGL_SHADER(
  
#ifdef GL_ES
precision highp float;
#else
#define lowp
#define mediump
#define highp
#endif

uniform sampler2D u_image;
varying vec2 v_pos;
uniform vec2 u_latrange;

uniform int u_mode;
uniform float u_mipmap;

uniform vec3 u_light;

uniform vec4 u_shadow;
uniform vec4 u_highlight;
uniform vec4 u_accent;

#define PI 3.141592653589793

const int mode_raw = 0;
const int mode_color = 1;

void main() {
    vec4 pixel = texture2D(u_image, v_pos);

    vec2 deriv = ((pixel.rg * 2.0) - 1.0);

    // we multiply the slope by a scale factor based on the cosin of the pixel's approximate latitude
    float scaleFactor = cos(radians((u_latrange[0] - u_latrange[1]) * (1.0 - v_pos.y) + u_latrange[1]));
    float slope = atan(1.25 * length(deriv)) / scaleFactor;
    float aspect = deriv.x != 0.0 ? atan(deriv.y, -deriv.x) : PI / 2.0 * (deriv.y > 0.0 ? 1.0 : -1.0);

    float intensity = u_light.x;
    // We add PI to make this angle match the global light object, which adds PI/2 to the light's azimuthal
    // position property to account for 0deg corresponding to north/the top of the viewport in the style spec
    // and the original shader was written to accept (-illuminationDirection - 90) as the azimuthal.
    float azimuth = u_light.y + PI;
    float polar = u_light.z;

    if (u_mode == mode_raw) {
        gl_FragColor = texture2D(u_image, v_pos);
    } else if (u_mode == mode_color) {
        float accent = cos(slope);
        vec4 accent_color = intensity * clamp((1.0 - accent) * 2.0, 0.0, 1.0) * u_accent;
        float shade = abs(mod((aspect + azimuth) / PI + 0.5, 2.0) - 1.0);

        vec4 shade_color = mix(u_shadow, u_highlight, shade) * slope * sin(polar) * mix(0.0, 2.0, intensity);
        gl_FragColor = accent_color * (1.0 - shade_color.a) + shade_color;
    } else {
        gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
    }

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

  )MBGL_SHADER";

  } // namespace shaders
  } // namespace mbgl
  