//Determine which texels are in the correct refinement level and output those levels into stencil

//eye space geometry mipmaps
uniform sampler2D MTEX_minmax_norm, MTEX_derivs;

//the thresholds for detecting depth and normal discontinuities
uniform float depthThreshold, normThreshold;

//maximum mipmap level in our multiresolution buffer
uniform float maxMipLevel;
uniform int fragMipLevel;

in vec2 fragTexCoord;
out vec4 out_FragColor;

//offset into texture
uniform float offset;

void main(void)
{

	// Get data passed down from program (the current mipmap level and
	//     a [0..1] xy-coordinate inside the mipmap level)
	float scale = 1.0;//1<<fragMipLevel;
	vec2 mipCoord = vec2(2.0*scale,scale)*fragTexCoord;// - vec2(scale*offset, 0.0);
	if(mipCoord.x >= 1.0) discard;
		
	// Compute the depth threhold for the current mipmap level and the next coarsest.
	//    If we used a constant depth threshold, this could simply be passed in from
	//    the user.
	float depthThresh0 = depthThreshold*(3.0/fragMipLevel);
	float depthThresh1 = depthThreshold*(3.0/(fragMipLevel+1));
	
	// All right, look up the depth discontinuties in this fragment (and it's next
	//    coarsest level in the mipmap)
	float depthDeriv0 = texture2DLod( MTEX_derivs, mipCoord.xy, fragMipLevel ).r;
	float depthDeriv1 = texture2DLod( MTEX_derivs, mipCoord.xy, fragMipLevel+1 ).r;
	
	// Look up the corresponding normal discontinities.
	vec4  minMaxNorm0 = texture2DLod( MTEX_minmax_norm, mipCoord.xy, fragMipLevel );
	vec4  minMaxNorm1 = texture2DLod( MTEX_minmax_norm, mipCoord.xy, fragMipLevel+1 );
	float normDeriv0 = max( minMaxNorm0.z-minMaxNorm0.x, minMaxNorm0.w-minMaxNorm0.y );
	float normDeriv1 = max( minMaxNorm1.z-minMaxNorm1.x, minMaxNorm1.w-minMaxNorm1.y );
	
	// If the either normal texture value == ( 1, 1, -1, -1 ) then that texel 
	//     is invalid and *must* be subdivided further.  If we encounter an invalid
	//     texel at the finest level, we may *discard* it, since it will not have any
	//     valid illumination.
	bool invalid0 = (normDeriv0 < -1) ? true : false;
	bool invalid1 = (normDeriv1 < -1) ? true : false;
	
	// If we are at the coarsest level of the finest level, we are at boundary cases.
	//    At the corsest level, we must handle the texel either here or at finer
	//    levels).  This means "Check 2" must always fail.  At the finest level,
	//    we must handle the texel either here or at coarser levels.  This means
	//    "Check 1 must always fail.
	bool atCoarsestLevel = ( fragMipLevel+1 > maxMipLevel ) ? true : false;
	bool atFinestLevel   = ( fragMipLevel <= 0.5 ) ? true : false;
	
	// Determine if we need additional refinement in this viscinity.
	bool needRefinment = (depthDeriv0 > depthThresh0) || (normDeriv0 > normThreshold);
	
	// Determine if the coarser level was sufficient.
	bool coarseLevelSufficient = (depthDeriv1 <= depthThresh1) && (normDeriv1 <= normThreshold) && !invalid1;
	
	// Check 1:  Do we need further refinement?  If so, we're not needed.
	if (needRefinment && !atFinestLevel) 
		discard;
		
	// Check 2:  Is the coarser resolution sufficient?  If so, we're not needed.
	if (coarseLevelSufficient && !atCoarsestLevel)
		discard;
		
	// Check 3:  Do we have an invalid texel?  If so it must be refined.  If
	//    we are at the coarsest level, we may safely discard an invalid texel
	//    because it will not contribute valid lighting to the scene (by def.)
	if (invalid0) 
	    discard;

	// OK.  We do not need further refinement and a coarser level does not suffice.
	//    The output color (at this point) does not matter.  We simply need to set the stencil.
	//out_FragColor = vec4( depthDeriv0 ); 
	out_FragColor = vec4(1.0 - 0.1*fragMipLevel ); 
}