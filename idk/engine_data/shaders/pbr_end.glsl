
	vec3 F = mix(vec3(0.04), albedo, metallic);
	vec3 kS = fresnelRoughness(max(dot(normal,view_dir), 0.0), F, roughness);
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;
	
	
	vec3 irradiance = texture(irradiance_probe, normal).rgb;
	vec3 diffuse = irradiance * albedo;
	
	const float MAX_REFLECTION_LOD = 4.0;
	
	//vec3 prefilteredColor = textureLod(environment_probe, reflected, roughness * MAX_REFLECTION_LOD).rgb;
	//vec2 envBRDF = texture(brdfLUT, vec2(max(dot(normal, view_dir), 0.0), roughness)).rg;
	//vec3 specular = prefilteredColor * (kS * envBRDF.x + envBRDF.y);
	//vec3 ambient = (kD * diffuse + specular) * vec3(0.01);
	vec3 ambient = vec3(0.03) * albedo;
	
	vec3 color = light_accum + ambient;
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2)); 
	out_color = vec4(color,1);	