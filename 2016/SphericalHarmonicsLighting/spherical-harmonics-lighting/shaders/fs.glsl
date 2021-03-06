
//////////////////////////////// Fragment shader
#version 330

#include "../spherical-harmonics-lighting/shaders/common/common.glsl"
#include "../spherical-harmonics-lighting/shaders/common/uvtile.glsl"
#include "../spherical-harmonics-lighting/shaders/common/parallax.glsl"

in vec3 iFS_Normal;
in vec2 iFS_UV;
in vec3 iFS_Tangent;
in vec3 iFS_Binormal;
in vec3 iFS_PointWS;

out vec4 ocolor0;

uniform vec3 Lamp0Pos = vec3(0.0,0.0,70.0);
uniform vec3 Lamp0Color = vec3(1.0,1.0,1.0);
uniform float Lamp0Intensity = 1.0;
uniform vec3 Lamp1Pos = vec3(70.0,0.0,0.0);
uniform vec3 Lamp1Color = vec3(0.198,0.198,0.198);
uniform float Lamp1Intensity = 1.0;

uniform float AmbiIntensity = 1.0;
uniform float EmissiveIntensity = 1.0;

uniform int parallax_mode = 0;

uniform float tiling = 1.0;
uniform vec3 uvwScale = vec3(1.0, 1.0, 1.0);
uniform bool uvwScaleEnabled = false;
uniform float envRotation = 0.0;
uniform float tessellationFactor = 4.0;
uniform float heightMapScale = 1.0;
uniform bool flipY = true;
uniform bool perFragBinormal = true;
uniform bool sRGBBaseColor = true;

uniform sampler2D heightMap;
uniform sampler2D normalMap;
uniform sampler2D baseColorMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform sampler2D emissiveMap;
uniform sampler2D specularLevel;
uniform sampler2D opacityMap;
uniform samplerCube environmentMap;

uniform mat4 viewInverseMatrix;

// Number of miplevels in the envmap
uniform float maxLod = 12.0;

// Actual number of samples in the table
uniform int nbSamples = 16;

// Irradiance spherical harmonics polynomial coefficients
// This is a color 2nd degree polynomial in (x,y,z), so it needs 10 coefficients
// for each color channel
uniform vec3 shCoefs[16];


// This must be included after the declaration of the uniform arrays since they
// can't be passed as functions parameters for performance reasons (on macs)
#include "../spherical-harmonics-lighting/shaders/common/pbr_ibl.glsl"


float fit_roughness(float r)
{
// Fit roughness values to a more usable curve
	return r;
}

void main()
{
	vec3 normalWS = iFS_Normal;
	vec3 tangentWS = iFS_Tangent;
	vec3 binormalWS = perFragBinormal ?
		fixBinormal(normalWS,tangentWS,iFS_Binormal) : iFS_Binormal;

	vec3 cameraPosWS = viewInverseMatrix[3].xyz;
	vec3 pointToLight0DirWS = Lamp0Pos - iFS_PointWS;
	float pointToLight0Length = length(pointToLight0DirWS);
	pointToLight0DirWS *= 1.0 / pointToLight0Length;
	vec3 pointToLight1DirWS = Lamp1Pos - iFS_PointWS;
	float pointToLight1Length = length(Lamp1Pos - iFS_PointWS);
	pointToLight1DirWS *= 1.0 / pointToLight1Length;
	vec3 pointToCameraDirWS = normalize(cameraPosWS - iFS_PointWS);

	// ------------------------------------------
	// Parallax

	vec2 uv;


	vec2 uvScale = vec2(1.0, 1.0);
	if (uvwScaleEnabled)
		uvScale = uvwScale.xy;

	uv = parallax_mode == 1 ? iFS_UV*tiling*uvScale : updateUV(
		heightMap,
		pointToCameraDirWS,
		normalWS, tangentWS, binormalWS,
		heightMapScale,
		iFS_UV,
		uvScale,
		tiling);


	uv = uv / tiling / uvScale;
	bool disableFragment = hasToDisableFragment(uv);
	uv = uv * tiling * uvScale;
	uv = getUDIMTileUV(uv);

	// ------------------------------------------
	// Add Normal from normalMap
	vec3 fixedNormalWS = normalWS;  // HACK for empty normal textures
	vec3 normalTS = get2DSample(normalMap, uv, disableFragment, cDefaultColor.mNormal).xyz;
	if(length(normalTS)>0.0001)
	{
		normalTS = fixNormalSample(normalTS,flipY);
		fixedNormalWS = normalize(
			normalTS.x*tangentWS +
			normalTS.y*binormalWS +
			normalTS.z*normalWS );
	}

	// ------------------------------------------
	// Compute material model (diffuse, specular & roughness)
	float dielectricSpec = get2DSample(specularLevel, uv, disableFragment, cDefaultColor.mSpecularLevel).r;
	dielectricSpec = 0.08 * dielectricSpec;
	vec3 dielectricColor = vec3(dielectricSpec, dielectricSpec, dielectricSpec);
	const float minRoughness=1e-4;
	// Convert the base color from sRGB to linear (we should have done this when
	// loading the texture but there is no way to specify which colorspace is
	// uѕed for a given texture in Designer yet)
	vec3 baseColor = get2DSample(baseColorMap, uv, disableFragment, cDefaultColor.mBaseColor).rgb;
	if (sRGBBaseColor)
		baseColor = srgb_to_linear(baseColor);


	float metallic = get2DSample(metallicMap, uv, disableFragment, cDefaultColor.mMetallic).r;
	float roughness = get2DSample(roughnessMap, uv, disableFragment, cDefaultColor.mRoughness).r;
	roughness = max(minRoughness, fit_roughness(roughness));

	vec3 diffColor = baseColor * (1.0 - metallic);
	vec3 specColor = mix(dielectricColor, baseColor, metallic);

	// ------------------------------------------
	// Compute point lights contributions
	vec3 contrib0 = pointLightContribution(
			fixedNormalWS,
			pointToLight0DirWS,
			pointToCameraDirWS,
			diffColor,
			specColor,
			roughness,
			Lamp0Color,
			Lamp0Intensity,
			pointToLight0Length );
	vec3 contrib1 = pointLightContribution(
			fixedNormalWS,
			pointToLight1DirWS,
			pointToCameraDirWS,
			diffColor,
			specColor,
			roughness,
			Lamp1Color,
			Lamp1Intensity,
			pointToLight1Length );

	// ------------------------------------------
	// Image based lighting contribution
	float ao = get2DSample(aoMap, uv, disableFragment, cDefaultColor.mAO).r;

	vec3 contribE = computeIBL(
		environmentMap, envRotation, maxLod,
		nbSamples,
		normalWS, fixedNormalWS, tangentWS, binormalWS,
		pointToCameraDirWS,
		diffColor, specColor, roughness,
		AmbiIntensity * ao);

	// ------------------------------------------
	//Emissive
	vec3 emissiveContrib = get2DSample(emissiveMap, uv, disableFragment, cDefaultColor.mEmissive).rgb;
	emissiveContrib = srgb_to_linear(emissiveContrib) * EmissiveIntensity;

	// ------------------------------------------
	vec3 finalColor = contrib0 + contrib1 + contribE + emissiveContrib;

	// Final Color
	// Convert the fragment color from linear to sRGB for display (we should
	// make the framebuffer use sRGB instead).
	//sfloat opacity = get2DSample(opacityMap, uv, disableFragment, cDefaultColor.mOpacity).r;
	ocolor0 = vec4(finalColor, 1.0);
}

