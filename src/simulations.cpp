#include <string>
#include "simulations.hpp"

void simulation_0d()
{
	//FHNOdeSystem ode;
	myFHNOdeSystem ode;
	//EulerIvpOdeSolver euler_solver;
	RungeKutta4IvpOdeSolver rk_solver;

	/* const auto sysParams = toml::parse(
		FHN_ODE_SYSTEM_CONSTANTS::CONFIG_PATH); */
	const auto sysParams = toml::parse(
		myFHN_ODE_SYSTEM_CONSTANTS::CONFIG_PATH);
	

	auto initial_conditions = toml::find<std::vector<double>>(
		sysParams, "initial_conditions");

	const double start_time = toml::find<double>(sysParams, "start_time");
	const double end_time = toml::find<double>(sysParams, "end_time");
	const double timestep = toml::find<double>(sysParams, "timestep");
	const double sampling_step = toml::find<double>(sysParams, 
		"sampling_step");

	std::cout << "Running 0D myFHN simulation..." << std::endl;

	/* OdeSolution solution = euler_solver.Solve(&ode, initial_conditions, 
		start_time, end_time, timestep, sampling_step); */

	OdeSolution solution = rk_solver.Solve(&ode, initial_conditions, 
		start_time, end_time, timestep, sampling_step);		

	solution.WriteToFile("0d_myFHN_simulation", "ode_solution", "sec");
}


void simulation_2d()
{
	// Get parameters from config file
	std::string param_file = USMC_2D_SYSTEM_CONSTANTS::CONFIG_DIR + 
		USMC_2D_SYSTEM_CONSTANTS::GENERAL_PARAM_FILE;
	const auto sys_params = toml::parse(param_file);

	// Time constants
	const double sim_duration = toml::find<double>(sys_params, "sim_duration");
	const double ode_timestep = toml::find<double>(sys_params, "ode_timestep");
	const double pde_timestep = toml::find<double>(sys_params, "pde_timestep");
	const double print_timestep = toml::find<double>(sys_params, 
		"print_timestep");

	const std::string mesh_dir = getenv("CHASTE_SOURCE_DIR") + 
		toml::find<std::string>(sys_params, "mesh_dir");
	const std::string mesh_name = toml::find<std::string>(sys_params,
		"mesh_name");
	const std::string cell_type = toml::find<std::string>(sys_params, 
		"cell_type");
	//test for 2nd cell type
	// try{
	// 	const std::string cell_type2 = toml::find<std::string>(sys_params, 
	// 	"cell_type2");
	// }
	// catch {
	// 	std::cout << "No cell_type2\n";
	// }
	//if not empty...indicate a 2nd cell included
	// if (!cell_type2.empty()){
	// 	std::cout << "Cell type 2 indicated; type " << cell_type2 << "\n\n";
	// }
	const std::string save_dir = toml::find<std::string>(sys_params,
		"save_dir"); // Top folder to save results
	const std::string stimulus_type = toml::find<std::string>(sys_params,
		"stimulus_type"); // Regular, simple or zero stimulus

	//below '3d' looks like a typo
	// std::string cell_param_file = USMC_3D_SYSTEM_CONSTANTS::CONFIG_DIR + 
	// 	cell_type + ".toml";
	std::string cell_param_file = USMC_2D_SYSTEM_CONSTANTS::CONFIG_DIR + 
		cell_type + ".toml";
	const auto cell_params = toml::parse(cell_param_file);

	// //if 2nd cell type, load parms here
	// if (!cell_type2.empty()) {
	// 	std::cout << "Loading parms for cell type 2...\n";
	
	// }

	// Cell parameters
	auto conductivities = toml::find<std::vector<double>>(
		cell_params, "conductivities_2d");	
	const double capacitance = toml::find<double>(cell_params, "capacitance");
	
	std::string default_ionic_model = cell_type + "I";
	std::string save_path = save_dir + "/" + cell_type + "/" + stimulus_type;

	// Log file location
	std::string log_dir = save_dir + "/" + cell_type + "/log";
	OutputFileHandler output_file_handler(log_dir, false);
	std::string log_path = output_file_handler.GetOutputDirectoryFullPath() + "log.log";

	HeartConfig::Instance()->SetSimulationDuration(sim_duration); //ms
	HeartConfig::Instance()->SetMeshFileName(mesh_dir + mesh_name);
	HeartConfig::Instance()->SetOutputDirectory(save_path);
	HeartConfig::Instance()->SetOutputFilenamePrefix("results");

	HeartConfig::Instance()->SetVisualizeWithVtk(true);

	AbstractUterineCellFactory *factory = NULL;

	if (stimulus_type == "simple")
	{
		factory = new UterineSimpleCellFactory();
	}

	else if (stimulus_type == "regular")
	{
		factory = new UterineRegularCellFactory();
	}

	else if (stimulus_type == "zero")
	{
		factory = new UterineZeroCellFactory();
	}

	else
	{
		const std::string err_message = "Unrecognized stimulus type";
		const std::string err_filename = "simulations.cpp";
		unsigned line_number = 96;

		throw Exception(err_message, err_filename, line_number);
	}

	MonodomainProblem<2> monodomain_problem(factory);

	HeartConfig::Instance()->SetIntracellularConductivities(Create_c_vector(
		conductivities[0], conductivities[1]));

	// HeartConfig::Instance()->SetSurfaceAreaToVolumeRatio(1400); // 1/cm
	HeartConfig::Instance()->SetCapacitance(capacitance); // uF/cm^2

	HeartConfig::Instance()->SetOdePdeAndPrintingTimeSteps(ode_timestep, 
		pde_timestep, print_timestep);
	HeartConfig::Instance()->SetDefaultIonicModel(default_ionic_model);

	// HeartConfig::Instance()->

	// Print information on the simulation to screen
	std::cout << "\n\n(simulations.cpp) Running 2D simulation..." << std::endl;
	std::cout << "\n(simulations.cpp) Using parm file: " << param_file << std::endl;

	std::cout << "\n(simulations.cpp) Stimulus type: " << stimulus_type << std::endl;
	std::cout << "\n(simulations.cpp) uSMC cell factory parameters:\n" << std::endl;
	factory->PrintParams();
	std::cout << "(simulations.cpp) x axis conductivity = " << conductivities[0] << std::endl;
	std::cout << "(simulations.cpp) y axis conductivity = " << conductivities[1] << std::endl;

	std::cout << std::endl;

	monodomain_problem.Initialise();
	monodomain_problem.Solve();

	HeartEventHandler::Headings();
	HeartEventHandler::Report();

	// Write to log file
	std::ofstream log_stream;
	log_stream.open(log_path, ios::app);

	log_stream << "System information" << std::endl;
	log_stream << "  cell type: " <<  cell_type << std::endl;
	log_stream << "  mesh: " << mesh_name << std::endl;
	log_stream << "  capacitance: " << capacitance << " uF/cm2" << std::endl;
	log_stream << "  conductivity x axis: " << conductivities[0] << std::endl;
	log_stream << "  conductivity y axis: " << conductivities[1] << std::endl;

	log_stream << "Simulation parameters" << std::endl;
	log_stream << "  duration: " << sim_duration << " ms" << std::endl;
	log_stream << "  ode timestep: " << ode_timestep << " ms" << std::endl;
	log_stream << "  pde timestep: " << pde_timestep << " ms" << std::endl;
	log_stream << "  print timestep: " << print_timestep << " ms" << std::endl;

	log_stream.close();

	factory->WriteLogInfo(log_path);
}

void simulation_2d_2type()
{
	//defining this for two cell types runsinto issue with 'only 3d supported' for key function in 
	//SetIonicModelRegions, of HeartConfig.hpp: "Unlike the get method, this is currently only supported in 3d."
	// Get parameters from config file
	std::string param_file = USMC_2D_SYSTEM_CONSTANTS::CONFIG_DIR + 
		USMC_2D_SYSTEM_CONSTANTS::GENERAL_2TYPE_PARAM_FILE;
	// std::string param_file = USMC_2D_SYSTEM_CONSTANTS::CONFIG_DIR + 
	// 	USMC_2D_SYSTEM_CONSTANTS::GENERAL_PARAM_FILE;

	std::cout << "(simulations.cpp) 2D 2-type simulation parm file: " << param_file << std::endl; 

	const auto sys_params = toml::parse(param_file);



	// Time constants
	const double sim_duration = toml::find<double>(sys_params, "sim_duration");
	const double ode_timestep = toml::find<double>(sys_params, "ode_timestep");
	const double pde_timestep = toml::find<double>(sys_params, "pde_timestep");
	const double print_timestep = toml::find<double>(sys_params, 
		"print_timestep");

	const std::string mesh_dir = getenv("CHASTE_SOURCE_DIR") + 
		toml::find<std::string>(sys_params, "mesh_dir");
	const std::string mesh_name = toml::find<std::string>(sys_params,
		"mesh_name");
	const std::string cell_type = toml::find<std::string>(sys_params, 
		"cell_type");

	const std::string cell_type2 = toml::find<std::string>(sys_params, 
		"cell_type2");	

	const std::string save_dir = toml::find<std::string>(sys_params,
		"save_dir"); // Top folder to save results
	const std::string stimulus_type = toml::find<std::string>(sys_params,
		"stimulus_type"); // Regular, simple or zero stimulus

	std::string cell_param_file = USMC_2D_SYSTEM_CONSTANTS::CONFIG_DIR + 
		cell_type + ".toml";
	const auto cell_params = toml::parse(cell_param_file);

	std::string cell2_param_file = USMC_2D_SYSTEM_CONSTANTS::CONFIG_DIR + 
		cell_type2 + ".toml";
	const auto cell2_params = toml::parse(cell2_param_file);	

	// Cell parameters
	auto conductivities = toml::find<std::vector<double>>(
		cell_params, "conductivities_2d");	
	const double capacitance = toml::find<double>(cell_params, "capacitance");
	
	std::string default_ionic_model = cell_type + "I";
	std::string save_path = save_dir + "/" + cell_type + "_" + cell_type2 + "/" + stimulus_type;

	// Log file location
	//std::string log_dir = save_dir + "/" + cell_type + "/log";
	std::string log_dir = save_dir + "/" + cell_type + "_" + cell_type2 + "/log";
	OutputFileHandler output_file_handler(log_dir, false);
	std::string log_path = output_file_handler.GetOutputDirectoryFullPath() + "log.log";


	HeartConfig::Instance()->SetSimulationDuration(sim_duration); //ms
	HeartConfig::Instance()->SetMeshFileName(mesh_dir + mesh_name);
	HeartConfig::Instance()->SetOutputDirectory(save_path);
	HeartConfig::Instance()->SetOutputFilenamePrefix("results");

	HeartConfig::Instance()->SetVisualizeWithVtk(true);

	AbstractUterineCellFactory *factory = NULL;

	if (stimulus_type == "simple")
	{
		factory = new UterineSimpleCellFactory();
	}

	else if (stimulus_type == "regular")
	{
		factory = new UterineRegularCellFactory();
	}

	else if (stimulus_type == "zero")
	{
		factory = new UterineZeroCellFactory();
	}

	else
	{
		const std::string err_message = "Unrecognized stimulus type";
		const std::string err_filename = "simulations.cpp";
		unsigned line_number = 96;

		throw Exception(err_message, err_filename, line_number);
	}

	MonodomainProblem<2> monodomain_problem(factory);

	HeartConfig::Instance()->SetIntracellularConductivities(Create_c_vector(
		conductivities[0], conductivities[1]));

	// HeartConfig::Instance()->SetSurfaceAreaToVolumeRatio(1400); // 1/cm
	HeartConfig::Instance()->SetCapacitance(capacitance); // uF/cm^2

	HeartConfig::Instance()->SetOdePdeAndPrintingTimeSteps(ode_timestep, 
		pde_timestep, print_timestep);
	HeartConfig::Instance()->SetDefaultIonicModel(default_ionic_model);

	// Print information on the simulation to screen
	std::cout << "\n\n(simulations.cpp) Running 2D simulation with 2 cell types..." << std::endl;
	std::cout << "\n(simulations.cpp) Using parm file: " << param_file << std::endl;

	std::cout << "\n(simulations.cpp) Stimulus type: " << stimulus_type << std::endl;
	std::cout << "\n(simulations.cpp) uSMC cell factory parameters:\n" << std::endl;
	factory->PrintParams();
	std::cout << "(simulations.cpp) x axis conductivity = " << conductivities[0] << std::endl;
	std::cout << "(simulations.cpp) y axis conductivity = " << conductivities[1] << std::endl;

	std::cout << std::endl;

	monodomain_problem.Initialise();
	monodomain_problem.Solve();

	HeartEventHandler::Headings();
	HeartEventHandler::Report();

	// Write to log file
	std::ofstream log_stream;
	log_stream.open(log_path, ios::app);

	log_stream << "System information" << std::endl;
	log_stream << "  cell type: " <<  cell_type << std::endl;
	log_stream << "  mesh: " << mesh_name << std::endl;
	log_stream << "  capacitance: " << capacitance << " uF/cm2" << std::endl;
	log_stream << "  conductivity x axis: " << conductivities[0] << std::endl;
	log_stream << "  conductivity y axis: " << conductivities[1] << std::endl;

	log_stream << "Simulation parameters" << std::endl;
	log_stream << "  duration: " << sim_duration << " ms" << std::endl;
	log_stream << "  ode timestep: " << ode_timestep << " ms" << std::endl;
	log_stream << "  pde timestep: " << pde_timestep << " ms" << std::endl;
	log_stream << "  print timestep: " << print_timestep << " ms" << std::endl;

	log_stream.close();

	factory->WriteLogInfo(log_path);	

}


void simulation_3d()
{
	// Get parameters from config file
	std::string param_file = USMC_3D_SYSTEM_CONSTANTS::CONFIG_DIR + 
		USMC_3D_SYSTEM_CONSTANTS::GENERAL_PARAM_FILE;
		
	const auto sys_params = toml::parse(param_file);

	// Time constants
	const double sim_duration = toml::find<double>(sys_params, "sim_duration");
	const double ode_timestep = toml::find<double>(sys_params, "ode_timestep");
	const double pde_timestep = toml::find<double>(sys_params, "pde_timestep");
	const double print_timestep = toml::find<double>(sys_params, 
		"print_timestep");

	const std::string mesh_dir = getenv("CHASTE_SOURCE_DIR") + 
		toml::find<std::string>(sys_params, "mesh_dir");
	const std::string mesh_name = toml::find<std::string>(sys_params,
		"mesh_name");
	const std::string cell_type = toml::find<std::string>(sys_params, 
		"cell_type");
	const std::string save_dir = toml::find<std::string>(sys_params,
		"save_dir"); // Top folder to save results
	const std::string stimulus_type = toml::find<std::string>(sys_params,
		"stimulus_type"); // Regular, simple or zero stimulus

	std::string cell_param_file = USMC_3D_SYSTEM_CONSTANTS::CONFIG_DIR + 
		cell_type + ".toml";
	const auto cell_params = toml::parse(cell_param_file);

	// Cell parameters
	auto conductivities = toml::find<std::vector<double>>(
		cell_params, "conductivities_3d");	
	const double capacitance = toml::find<double>(cell_params, "capacitance");
	
	std::string default_ionic_model = cell_type + "I";
	std::string save_path = save_dir + "/" + cell_type + "/" + stimulus_type;

	// Log file location
	std::string log_dir = save_dir + "/" + cell_type + "/log";
	OutputFileHandler output_file_handler(log_dir, false);
	std::string log_path = output_file_handler.GetOutputDirectoryFullPath() + "log.log";

	HeartConfig::Instance()->SetSimulationDuration(sim_duration); //ms
	HeartConfig::Instance()->SetMeshFileName(mesh_dir + mesh_name);
	HeartConfig::Instance()->SetOutputDirectory(save_path);
	HeartConfig::Instance()->SetOutputFilenamePrefix("results");

	HeartConfig::Instance()->SetVisualizeWithVtk(true);

	AbstractUterineCellFactory3d *factory = NULL;

	if (stimulus_type == "simple")
	{
		factory = new UterineSimpleCellFactory3d();
	}

	else if (stimulus_type == "regular")
	{
		factory = new UterineRegularCellFactory3d();
	}

	else if (stimulus_type == "zero")
	{
		factory = new UterineZeroCellFactory3d();
	}

	else
	{
		const std::string err_message = "Unrecognized stimulus type";
		const std::string err_filename = "simulations.cpp";
		unsigned line_number = 220;

		throw Exception(err_message, err_filename, line_number);
	}

	MonodomainProblem<3> monodomain_problem(factory);

	HeartConfig::Instance()->SetIntracellularConductivities(Create_c_vector(
		conductivities[0], conductivities[1], conductivities[2]));

	// HeartConfig::Instance()->SetSurfaceAreaToVolumeRatio(1400); // 1/cm
	HeartConfig::Instance()->SetCapacitance(capacitance); // uF/cm^2

	HeartConfig::Instance()->SetOdePdeAndPrintingTimeSteps(ode_timestep, 
		pde_timestep, print_timestep);
	HeartConfig::Instance()->SetDefaultIonicModel(default_ionic_model);

	// Print information on the simulation to screen
	std::cout << "Running 3D simulation..." << std::endl;

	std::cout << "\nStimulus type: " << stimulus_type << std::endl;
	std::cout << "\nuSMC cell factory parameters:\n" << std::endl;
	factory->PrintParams();
	std::cout << "x axis conductivity = " << conductivities[0] << std::endl;
	std::cout << "y axis conductivity = " << conductivities[1] << std::endl;
	std::cout << "z axis conductivity = " << conductivities[2] << std::endl;

	std::cout << std::endl;

	monodomain_problem.Initialise();
	monodomain_problem.Solve();

	HeartEventHandler::Headings();
	HeartEventHandler::Report();

	// Write to log file
	std::ofstream log_stream;
	log_stream.open(log_path, ios::app);

	log_stream << "System information" << std::endl;
	log_stream << "  cell type: " <<  cell_type << std::endl;
	log_stream << "  mesh: " << mesh_name << std::endl;
	log_stream << "  capacitance: " << capacitance << " uF/cm2" << std::endl;
	log_stream << "  conductivity x axis: " << conductivities[0] << std::endl;
	log_stream << "  conductivity y axis: " << conductivities[1] << std::endl;
	log_stream << "  conductivity z axis: " << conductivities[2] << std::endl;

	log_stream << "Simulation parameters" << std::endl;
	log_stream << "  duration: " << sim_duration << " ms" << std::endl;
	log_stream << "  ode timestep: " << ode_timestep << " ms" << std::endl;
	log_stream << "  pde timestep: " << pde_timestep << " ms" << std::endl;
	log_stream << "  print timestep: " << print_timestep << " ms" << std::endl;

	log_stream.close();

	factory->WriteLogInfo(log_path);
}

void simulation_3d_2type()
{
	// Get parameters from config file
	std::string param_file = USMC_3D_2TYPE_SYSTEM_CONSTANTS::CONFIG_DIR + 
		USMC_3D_2TYPE_SYSTEM_CONSTANTS::GENERAL_2TYPE_PARAM_FILE;
	// std::string param_file = USMC_3D_SYSTEM_CONSTANTS::CONFIG_DIR + 
	// 	USMC_3D_SYSTEM_CONSTANTS::GENERAL_PARAM_FILE;

	std::cout << "(simulations.cpp) 3D 2-type simulation parm file: " << param_file << std::endl; 	
		
	const auto sys_params = toml::parse(param_file);

	// Time constants
	const double sim_duration = toml::find<double>(sys_params, "sim_duration");
	const double ode_timestep = toml::find<double>(sys_params, "ode_timestep");
	const double pde_timestep = toml::find<double>(sys_params, "pde_timestep");
	const double print_timestep = toml::find<double>(sys_params, 
		"print_timestep");

	const std::string mesh_dir = getenv("CHASTE_SOURCE_DIR") + 
		toml::find<std::string>(sys_params, "mesh_dir");
	const std::string mesh_name = toml::find<std::string>(sys_params,
		"mesh_name");
	const std::string cell_type = toml::find<std::string>(sys_params, 
		"cell_type");
	const std::string cell_type2 = toml::find<std::string>(sys_params, 
		"cell_type2");			
	const std::string save_dir = toml::find<std::string>(sys_params,
		"save_dir"); // Top folder to save results
	const std::string stimulus_type = toml::find<std::string>(sys_params,
		"stimulus_type"); // Regular, simple or zero stimulus

	//conductivity modifier
	const std::string modifier_type = toml::find<std::string>(sys_params,
		"conductivity_mod_type");

	//compiler wants these declared and init before type check...?!
	//double conduct_mod_slope = -1;
	//double conduct_mod_max = -1;
	/* double conduct_mod_center = -1;
	double conduct_mod_steep = -1;
	double conduct_mod_min = -1; */

	//attempt parameter array 
	std::vector<double> cond_mod_parms;

	//if linear load linear parms
	if (modifier_type == "linear"){
		//compiler wants these declared and init before type check...?!
	/* 	double conduct_mod_slope = -1;
		double conduct_mod_max = -1; */		
		cond_mod_parms.push_back(toml::find<double>(sys_params, "conduct_mod_slope"));
		cond_mod_parms.push_back(toml::find<double>(sys_params, "conduct_mod_max"));
	}

	else if (modifier_type == "gaussian"){
		cond_mod_parms.push_back(toml::find<double>(sys_params, "conduct_mod_center"));
		cond_mod_parms.push_back(toml::find<double>(sys_params, "conduct_mod_steep"));
		cond_mod_parms.push_back(toml::find<double>(sys_params, "conduct_mod_min"));
		
	}


	std::string cell_param_file = USMC_3D_2TYPE_SYSTEM_CONSTANTS::CONFIG_DIR + 
		cell_type + ".toml";
	std::string cell_param_file2 = USMC_3D_2TYPE_SYSTEM_CONSTANTS::CONFIG_DIR + 
		cell_type2 + ".toml";	

	std::cout << "(simulations.cpp) cell_param_file: " << cell_param_file << std::endl;
	std::cout << "(simulations.cpp) cell_param_file2: " << cell_param_file2 << std::endl;

	const auto cell_params = toml::parse(cell_param_file);

	const auto cell_params2 = toml::parse(cell_param_file2);

	// Cell parameters
	auto conductivities = toml::find<std::vector<double>>(
		cell_params, "conductivities_3d");	
	const double capacitance = toml::find<double>(cell_params, "capacitance");

	auto conductivities2 = toml::find<std::vector<double>>(
		cell_params2, "conductivities_3d");	
	const double capacitance2 = toml::find<double>(cell_params2, "capacitance");	
	
	//below is deprecated...? default_ionic_model doesn't do anything
	std::string default_ionic_model = cell_type + "I";
	//std::string save_path = save_dir + "/" + cell_type + "/" + stimulus_type;

	//OutputFileHandler output_file_handler(log_dir, false);

	//shouldn't have to do this...some env var is out of whack?
	//std::string save_path_get = output_file_handler.GetOutputDirectoryFullPath(); 
	//std::cout << "(simulations.cpp) save_path_get:" << save_path_get << std::endl;

	std::string save_path = save_dir + "/" + cell_type + "_" + cell_type2 + "/" + stimulus_type;
	OutputFileHandler save_output_file_handler(save_path, false);
	std::string full_save_path = save_output_file_handler.GetOutputDirectoryFullPath();


	std::cout << "(simulations.cpp) save_path: " << save_path << std::endl;
	std::cout << "(simulations.cpp) full_save_path: " << full_save_path << std::endl;

	// Log file location
	//std::string log_dir = save_dir + "/" + cell_type + "/log";
	std::string log_dir = save_dir + "/" + cell_type + "_" + cell_type2 + "/log";
	OutputFileHandler output_file_handler(log_dir, false);
	std::string log_path = output_file_handler.GetOutputDirectoryFullPath() + "log.log";

	std::cout << "(simulations.cpp) log_path: " << log_path << std::endl;

	HeartConfig::Instance()->SetSimulationDuration(sim_duration); //ms
	HeartConfig::Instance()->SetMeshFileName(mesh_dir + mesh_name);
	HeartConfig::Instance()->SetOutputDirectory(save_path);
	HeartConfig::Instance()->SetOutputFilenamePrefix("results");

	HeartConfig::Instance()->SetVisualizeWithVtk(true);

	AbstractUterineCellFactory3d_2type *factory = NULL;

	if (stimulus_type == "simple")
	{
		factory = new UterineSimpleCellFactory3d_2type();
	}

/* 	else if (stimulus_type == "regular")
	{
		factory = new UterineRegularCellFactory3d();
	}
*/
	else if (stimulus_type == "zero")
	{
		factory = new UterineZeroCellFactory3d_2type();
	} 

	else
	{
		const std::string err_message = "Unrecognized stimulus type";
		const std::string err_filename = "simulations.cpp";
		unsigned line_number = 220;

		throw Exception(err_message, err_filename, line_number);
	}

	MonodomainProblem<3> monodomain_problem(factory);
	//std::cout << "\n(simulations.cpp) factory->IsMeshProvided()" << factory->IsMeshProvided() << std::endl;

	/* HeartConfig::Instance()->SetIntracellularConductivities(Create_c_vector(
		conductivities[0], conductivities[1], conductivities[2])); */



	// HeartConfig::Instance()->SetSurfaceAreaToVolumeRatio(1400); // 1/cm
	HeartConfig::Instance()->SetCapacitance(capacitance); // uF/cm^2

	HeartConfig::Instance()->SetOdePdeAndPrintingTimeSteps(ode_timestep, 
		pde_timestep, print_timestep);
	//HeartConfig::Instance()->SetDefaultIonicModel(default_ionic_model);

	//define region for 2nd cell type
	std::vector<ChasteCuboid<3> > region_2type;
	ChastePoint<3> corner_a(0, 0, 0);
    ChastePoint<3> corner_b(1, 1, 1);

	//region_2type.push_back(ChasteCuboid<3> (corner_a,corner_b));

	//test defining cp:ionicmodel_selection_type - SetIonicModelRegions wants this type...?
	//cp::ionic_model_selection_type second_ionic_model= cell_type2;
	//std::string second_ionic_model = cell_type2 + "I";
	


	//second_ionic_model = cell_type2 + "I";

	//create vector of 2nd region cell model strings
	//std::vector<cp::ionic_model_selection_type> cell_models_2type;

	//append the 2nd model to the model vector
	//cell_models_2type.push_back(default_ionic_model);
	//cell_models_2type[0] = default_ionic_model;

	//define 2nd Ionic Model / Region
	//HeartConfig::Instance()->SetIonicModelRegions(region_2type, cell_models_2type);


	// Print information on the simulation to screen
	std::cout << "\n\n(simulations.cpp) Running 3D simulation with 2 cell types.." << std::endl;
	std::cout << "\n(simulations.cpp) Using parm file: " << param_file << std::endl;	
	std::cout << "\n\tCell type 1: " << cell_type << "Cell type 2: " << cell_type2 << std::endl;

	std::cout << "\n(simulations.cpp) Simulation duration: " << sim_duration << std::endl;

	std::cout << "\n(simulations.cpp) Stimulus type: " << stimulus_type << std::endl;
	std::cout << "\n(simulations.cpp) uSMC cell factory parameters:\n" << std::endl;
	factory->PrintParams();
	std::cout << "(simulations.cpp) x axis conductivity cell1 = " << conductivities[0] << std::endl;
	std::cout << "(simulations.cpp) y axis conductivity cell1 = " << conductivities[1] << std::endl;
	std::cout << "(simulations.cpp) z axis conductivity cell1 = " << conductivities[2] << std::endl;
	std::cout << "(simulations.cpp) capacitance cell1 = " << capacitance << std::endl;

	std::cout << "(simulations.cpp) x axis conductivity cell2 = " << conductivities2[0] << std::endl;
	std::cout << "(simulations.cpp) y axis conductivity cell2 = " << conductivities2[1] << std::endl;
	std::cout << "(simulations.cpp) z axis conductivity cell2 = " << conductivities2[2] << std::endl;
	std::cout << "(simulations.cpp) capacitance cell2 = " << capacitance2 << std::endl;

	std::cout << "(simulations.cpp) conductivity modifier type = " << modifier_type << std::endl;


	//std::cout << "(simulations.cpp) 2nd Region Cuboid corners: \n\t[" << corner_a[0] << " " << corner_a[1] << " " << corner_a[2] << "]\n\t" 
	//	<< corner_b[0] << " " << corner_b[1] << " " << corner_b[2] << "]" << std::endl;

	std::cout << std::endl;

	monodomain_problem.Initialise();

	//modify conductance - with our multi-type version
	UterineConductivityModifier modifier;

	/* //test modifying the conductances
	LinearConductivityModifier lin_modifier; 
	GaussianConductivityModifier gau_modifier;  */

	//for linear distn of conductance
	/* modifier.linear_conduct_z_slope = -0.025;
	modifier.linear_conduct_max = conductivities[2]; */
	if (modifier_type == "linear"){	
		//destroy gau_modifier;
		//modifier.linear_conduct_z_slope = cond_mod_parms[0];
		//modifier.linear_conduct_max = cond_mod_parms[1]; 

		//display these modifier parms to screen
		std::cout << "(simulations.cpp) conductivity modifier type: = " << modifier_type << std::endl;
		std::cout << "(simulations.cpp) conductivity modifer slope: = " << cond_mod_parms[0] << std::endl;
		std::cout << "(simulations.cpp) conductivity modifer max: = " << cond_mod_parms[1] << std::endl;
	}
	else if (modifier_type == "gaussian"){
		modifier.gauss_z_center = cond_mod_parms[0];
		modifier.gauss_z_steep = cond_mod_parms[1];
		modifier.gauss_z_min = cond_mod_parms[2];

		//display these modifier parms to screen
		std::cout << "(simulations.cpp) conductivity modifier (gauss) center: = " << cond_mod_parms[0] << std::endl;
		std::cout << "(simulations.cpp) conductivity modifer (gauss)  steep: = " << cond_mod_parms[1] << std::endl;
		std::cout << "(simulations.cpp) conductivity modifer (gauss) min: = " << cond_mod_parms[2] << std::endl;		
	
	}		 
	



	//attempt modify conductances - get the tissue model
	MonodomainTissue<3>* p_monodomain_tissue = monodomain_problem.GetMonodomainTissue();

	//following https://chaste.github.io/docs/user-tutorials/bidomainwithconductivitymodifier/
	//this does the conductivity modification in the model
	p_monodomain_tissue->SetConductivityModifier( &modifier );

	//we can get the mesh directly from the problem struct - see https://chaste.github.io/docs/user-tutorials/monodomain3drabbitheart/
	//below is for visualisation -- for pulling _out_ the modified conductivities
	//AbstractTetrahedralMesh<3,3>* p_mesh = &(monodomain_problem.rGetMesh());
	modifier.p_mesh = &(monodomain_problem.rGetMesh());	

	//for writing out modifier distribution conductivities
	VtkMeshWriter<3,3> conductivities_mesh_writer(save_path+"_modifier","conductivity_mod",false);

	//for outputting the conductivities per element, need a c_vect sized numelements*space_dim
	//std::vector<c_vector<double, 3>> testcondata(modifier.p_mesh->GetNumAllElements());

	//temp_mesh_writer::AddTensorCellData("Conductivity",modifier);

	//TRY BELOW 'AddCellData' (or AddTensorCellData) to the element loop...and pass a c_matrix of the conductivity tensor...
	//temp_mesh_writer.AddCellData("Conductivity",);
	//temp_mesh_writer.WriteFilesUsingMesh(monodomain_problem.rGetMesh());

	//for element handling
	//Element<3,3>* p_element;

	//debug: display conductivities over all elements (don't do this for sims! :)
	std::cout << "(simulations.cpp) conductivities: " << std::endl;

	//instead of dumping to a log, output to a vtk file for visualisation
	//for mapping over the conductivity tensor to output var
	c_vector<double, 3> cur_conduct;
	std::vector< c_vector<double,3> > mod_conductivities;

	for (AbstractTetrahedralMesh<3,3>::ElementIterator elt_iter=modifier.p_mesh->GetElementIteratorBegin();
        elt_iter!=modifier.p_mesh->GetElementIteratorEnd();
        ++elt_iter)
		{
			unsigned index = elt_iter->GetIndex();

			//simply print out the conductivities for the 'intracell' (monodomain)
    		//	std::cout << "     element id #" << index << std::endl;
			//spatial coords
			Element<3,3>* p_element = (modifier.p_mesh->GetElement(index));
			c_vector<double, 3> cur_centroid = p_element->CalculateCentroid();
			//std::cout << "          centroid: " << std::endl;
			//std::cout << "          x: " << cur_centroid(0) << " y: " << cur_centroid(1) << " z: " << cur_centroid(2) << std::endl; 

			//load up the conductivties for each element into our output array
			//conductivities - note chaste stores "index*conductivity" so we divide those out
			//that forces different printing for the 0th terms
			//std::cout << "          conductivities: " << std::endl;

			if (index == 0)
			{
				//
				/* std::cout << "          x: " << p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(0,0) << " y: " <<
			    	p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(1,1) << " z: " << 
					p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(2,2) << std::endl; */

				//test load into c_vector
				/* c_vector<double, 3> cur_conduct = {p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(0,0), 
										p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(1,1),
										p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(2,2)}; */
				
				cur_conduct[0] = p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(0,0);
				cur_conduct[1] = p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(1,1);
				cur_conduct[2] = p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(2,2);	
				mod_conductivities.push_back( cur_conduct);
				//std::fill_n(cur_conduct.begin(),3,0.0);				
				//instead of printing load into output array
				/* testcondata[index] = {p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(0,0), 
										p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(1,1),
										p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(2,2)}; */
			}
			else{
				//std::cout << "          conductivities: " << std::endl;
				//std::cout << "          x: " << p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(0,0)/index << " y: " <<
			    	//p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(1,1)/index << " z: " << 
					//p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(2,2)/index << std::endl;

				cur_conduct[0] = p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(0,0)/index;
				cur_conduct[1] = p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(1,1)/index;
				cur_conduct[2] = p_monodomain_tissue->rGetIntracellularConductivityTensor(index)(2,2)/index;
				mod_conductivities.push_back( cur_conduct);
			}

		} 
	
	//length of mod_conductivities should match num of elements
	//std::cout << "mod_conductivities length: " << mod_conductivities.size() << " number of elements: " << modifier.p_mesh->GetNumAllElements() << std::endl;

	//pass this vector<c_vector>) to AddCellData for output
	conductivities_mesh_writer.AddCellData("Conductivity Modifier",mod_conductivities);
	conductivities_mesh_writer.WriteFilesUsingMesh(monodomain_problem.rGetMesh());

	//monodomain_problem.rGetMesh();
	//mesh = monodomain_problem.rGetMesh();
	//std::cout << "(simulations.cpp) monodomain_problem.rGetMesh(): " << monodomain_problem.rGetMesh() << std::endl;
	//DistributedTetrahedralMesh<3,3> mesh;
	//mesh = monodomain_problem.Get_Mesh();
	


	monodomain_problem.Solve();

	HeartEventHandler::Headings();
	HeartEventHandler::Report();

	// Write to log file
	std::ofstream log_stream;
	log_stream.open(log_path, ios::app);

	//write the std out data as well as parms etc
	log_stream << "(simulations.cpp) 3D 2-type simulation parm file: " << param_file << std::endl;

	log_stream << std::endl;	

	log_stream << "(simulations.cpp) cell_param_file: " << cell_param_file << std::endl;
	log_stream << "(simulations.cpp) cell_param_file2: " << cell_param_file2 << std::endl;

	log_stream << std::endl;

	log_stream << "(simulations.cpp) save_path: " << save_path << std::endl;
	log_stream << "(simulations.cpp) full_save_path: " << full_save_path << std::endl;

	log_stream << std::endl;

	log_stream << "(simulations.cpp) log_path: " << log_path << std::endl;

	log_stream << std::endl;

	log_stream << "\n\n(simulations.cpp) Running 3D simulation with 2 cell types.." << std::endl;
	log_stream << "\n(simulations.cpp) Using parm file: " << param_file << std::endl;	
	log_stream << "\n\tCell type 1: " << cell_type << "Cell type 2: " << cell_type2 << std::endl;

	log_stream << "\n(simulations.cpp) Simulation duration: " << sim_duration << std::endl;

	log_stream << "\n(simulations.cpp) Stimulus type: " << stimulus_type << std::endl;
	log_stream << "\n(simulations.cpp) uSMC cell factory parameters:\n" << std::endl;
	factory->PrintParams();
	log_stream << "(simulations.cpp) x axis conductivity cell1 = " << conductivities[0] << std::endl;
	log_stream << "(simulations.cpp) y axis conductivity cell1 = " << conductivities[1] << std::endl;
	log_stream << "(simulations.cpp) z axis conductivity cell1 = " << conductivities[2] << std::endl;
	log_stream << "(simulations.cpp) capacitance cell1 = " << capacitance << std::endl;

	log_stream << "(simulations.cpp) x axis conductivity cell2 = " << conductivities2[0] << std::endl;
	log_stream << "(simulations.cpp) y axis conductivity cell2 = " << conductivities2[1] << std::endl;
	log_stream << "(simulations.cpp) z axis conductivity cell2 = " << conductivities2[2] << std::endl;
	log_stream << "(simulations.cpp) capacitance cell2 = " << capacitance2 << std::endl;

	log_stream << "(simulations.cpp) conductivity modifier type = " << modifier_type << std::endl;

	log_stream << std::endl;

	//for linear distn of conductance
	/* modifier.linear_conduct_z_slope = -0.025;
	modifier.linear_conduct_max = conductivities[2]; */
	if (modifier_type == "linear"){	
		//destroy gau_modifier;
		//modifier.linear_conduct_z_slope = cond_mod_parms[0];
		//modifier.linear_conduct_max = cond_mod_parms[1]; 

		//display these modifier parms to screen
		log_stream << "(simulations.cpp) conductivity modifier type: = " << modifier_type << std::endl;
		log_stream << "(simulations.cpp) conductivity modifer slope: = " << cond_mod_parms[0] << std::endl;
		log_stream << "(simulations.cpp) conductivity modifer max: = " << cond_mod_parms[1] << std::endl;
	}
	else if (modifier_type == "gaussian"){
		modifier.gauss_z_center = cond_mod_parms[0];
		modifier.gauss_z_steep = cond_mod_parms[1];
		modifier.gauss_z_min = cond_mod_parms[2];

		//display these modifier parms to screen
		log_stream << "(simulations.cpp) conductivity modifier (gauss) center: = " << cond_mod_parms[0] << std::endl;
		log_stream << "(simulations.cpp) conductivity modifer (gauss)  steep: = " << cond_mod_parms[1] << std::endl;
		log_stream << "(simulations.cpp) conductivity modifer (gauss) min: = " << cond_mod_parms[2] << std::endl;		
	
	}		 

	log_stream << std::endl;

	//debug: display conductivities over all elements (don't do this for sims! :)
	log_stream << "(simulations.cpp) conductivities: (if enabled LONG LIST)" << std::endl;

	log_stream << "System information" << std::endl;
	log_stream << "  cell type: " <<  cell_type << std::endl;
	log_stream << "  mesh: " << mesh_name << std::endl;
	log_stream << "  capacitance: " << capacitance << " uF/cm2" << std::endl;
	log_stream << "  conductivity x axis: " << conductivities[0] << std::endl;
	log_stream << "  conductivity y axis: " << conductivities[1] << std::endl;
	log_stream << "  conductivity z axis: " << conductivities[2] << std::endl;

	log_stream << "  conductivity modifier parms: " << std::endl;
	log_stream << "       slope: " << cond_mod_parms[0] << std::endl;
	log_stream << "       max:   " << cond_mod_parms[1] << std::endl;

	log_stream << "Simulation parameters" << std::endl;
	log_stream << "  duration: " << sim_duration << " ms" << std::endl;
	log_stream << "  ode timestep: " << ode_timestep << " ms" << std::endl;
	log_stream << "  pde timestep: " << pde_timestep << " ms" << std::endl;
	log_stream << "  print timestep: " << print_timestep << " ms" << std::endl;

	log_stream.close();

	factory->WriteLogInfo(log_path);

}
