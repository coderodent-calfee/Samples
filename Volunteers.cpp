#include <cmath>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <matplot/matplot.h>
#include <algorithm>
#include <random>

class Volunteer {
public: 
   Volunteer(size_t num, std::string named) :id(num), name(named) {}
   std::string name;
   std::vector<size_t> jobIds;
   size_t id;
};


class Job {
public:
   Job(size_t num, std::string named, size_t needed) : id(num), name(named), volunteersNeeded(needed) {}
   std::string name;
   size_t id, volunteersNeeded;
};

void doVolunteerJobs() {
   size_t maxNameSize = 0;

   // PREP
   std::vector<std::string> jobNames = { "Arrange Chairs", "Bring Snacks", "Clean Up", "Distribute Pamphlets", "Entertainment", "Food Prep" };
   std::vector<std::string> volunteerNames = { "Alice", "Brianna", "Charlie", "Doug", "Edward", "Foster", "Gregory"};
   
   size_t maxJobs = jobNames.size();
   int needed = 4;
   std::unordered_map<size_t, Job*> jobMap;
   for (size_t i = 0; i < maxJobs; i++ ) {
      auto job = new Job(i, jobNames[i], needed > 1?needed--:1);
      jobMap[job->id] = job;
   }

   size_t numVolunteers = volunteerNames.size();
   std::unordered_map<size_t, Volunteer*> volunteerMap;
   for (size_t i = 0; i < numVolunteers; i++) {
      auto volunteer = new Volunteer(i + maxJobs, volunteerNames[i]);
      maxNameSize = max(maxNameSize, volunteer->name.length());
      volunteerMap[volunteer->id] = volunteer;
      for (int j = 0; j < maxJobs; j++) {
         volunteer->jobIds.push_back(j);
      }
      unsigned seed = (unsigned)i;
      std::shuffle(volunteer->jobIds.begin(), volunteer->jobIds.end(), std::default_random_engine(seed));
      volunteer->jobIds.erase(volunteer->jobIds.begin(), volunteer->jobIds.begin() + 3);
   }

   //ASSIGN
   // ordered by weight
   std::vector<std::pair<size_t, size_t>> edges;
   std::vector<size_t> edgeWeights;
   std::vector<std::vector<size_t>> edgesGroupedByWeights;
   std::vector<size_t> edgeIndexByWeight;
   for (int i = 0; i < 5; i++) {
      edgesGroupedByWeights.push_back( std::vector<size_t>(0));
   }
//   std::fill_n(edgesGroupedByWeights.begin(), 5, std::vector<size_t>());

   // assemble job/volunteer edges (keep them indexed)
   for (auto volIter = volunteerMap.begin(); volIter != volunteerMap.end(); volIter++) {
      auto jobBegin = volIter->second->jobIds.begin();
      for (auto jobIter = jobBegin; jobIter != volIter->second->jobIds.end(); jobIter++) {
         size_t distance = jobIter - jobBegin;
         size_t desire = (distance < 3) ? 4 - distance : 1;
         auto edgeIndex = edges.size();
         edgeWeights.push_back(desire);
         edges.push_back({ volIter->second->id, *jobIter });
         // weight is backwards
         edgesGroupedByWeights[4-desire].push_back(edgeIndex);
      }
   }

   for (size_t i = 0; i < 4; i++) {
      edgeIndexByWeight.insert(edgeIndexByWeight.end(), edgesGroupedByWeights[i].begin(), edgesGroupedByWeights[i].end());
   }

   std::vector<size_t> assignedEdgeIndexByWeight;
   std::vector<size_t> assignedJobs(jobMap.size());
   std::fill_n(assignedJobs.begin(), jobMap.size(), 0);

   std::unordered_map<size_t, std::vector<size_t>> volunteerIdByJobId(jobMap.size());
   std::unordered_map<size_t, std::vector<size_t>> jobAssignmentsByVolunteerId(volunteerMap.size());

   for (size_t i = 0; i < maxJobs; i++) {
      volunteerIdByJobId.insert({i, std::vector<size_t>()});
   }

   for (auto it = volunteerMap.begin(); it != volunteerMap.end(); it++) {
      jobAssignmentsByVolunteerId.insert({ it->first, std::vector<size_t>() });
   }

   // standins?

   // iterate from highest weight to lowest
   // note job Id is the same as job index
   for (auto edgeIndexIter = edgeIndexByWeight.begin(); edgeIndexIter != edgeIndexByWeight.end(); edgeIndexIter++) {
      auto edge = edges[*edgeIndexIter];
      auto volunteerId = edge.first;
      auto jobId = edge.second;
      auto needed = jobMap[jobId]->volunteersNeeded;
      if (assignedJobs[jobId] < needed) {
         assignedEdgeIndexByWeight.push_back(*edgeIndexIter);
         assignedJobs[jobId]++;
         jobAssignmentsByVolunteerId[volunteerId].push_back(jobId);
         volunteerIdByJobId[jobId].push_back(volunteerId);
      }
      else {
         // add to standby list
      }
   }

   //DISPLAY
   std::vector<std::string> nodeNames;

   std::cout << "Volunteers: " << std::endl;
   for (auto volIter = volunteerMap.begin(); volIter != volunteerMap.end(); volIter++) {
      std::cout << volIter->first << " " << volIter->second->name << " wants to" << std::endl;
      auto jobBegin = volIter->second->jobIds.begin();
      for (auto jobIter = jobBegin; jobIter != volIter->second->jobIds.end(); jobIter++) {
         std::cout << "\t" << *jobIter << " " << jobMap[*jobIter]->name << std::endl;
      }
      std::cout << "Assigned to:" << std::endl;
      auto assignedJobs = jobAssignmentsByVolunteerId[volIter->second->id];
      for (auto jobIter = assignedJobs.begin(); jobIter != assignedJobs.end(); jobIter++) {
         std::cout << "\t" << jobMap[*jobIter]->name << std::endl;
      }
      std::cout << std::endl;
   }
   std::cout << std::endl;

   std::cout << "Jobs: " << std::endl;
   for (auto jobIter = jobMap.begin(); jobIter != jobMap.end(); jobIter++) {
      std::cout << jobIter->first << " " << jobIter->second->name << " needs " << jobIter->second->volunteersNeeded << ":" ;
      auto assignedVolunteers = volunteerIdByJobId[jobIter->second->id];
      for (auto volunteerIter = assignedVolunteers.begin(); volunteerIter != assignedVolunteers.end(); volunteerIter++) {
         std::cout << " " << volunteerMap[*volunteerIter]->name;
      }
      std::cout << std::endl;

   }
   std::cout << std::endl;

   std::cout << "Assignments: " << std::endl;
   for (auto assignmentIter = assignedEdgeIndexByWeight.begin(); assignmentIter != assignedEdgeIndexByWeight.end(); assignmentIter++) {
      auto assignedEdge = *assignmentIter;
      auto edge = edges[assignedEdge];

      auto volunteerId = edge.first;
      auto jobId = edge.second;
      auto volunteer = volunteerMap[volunteerId];
      auto job = jobMap[jobId];

      // TODO: use the precalculated weight
      auto jobBegin = volunteer->jobIds.begin();
      auto jobIter = find(jobBegin, volunteer->jobIds.end(), jobId);
      size_t distance = jobIter - jobBegin;
      size_t desire = (distance < 3) ? 4 - distance : 1;

      std::cout << std::setfill(' ') << std::setw(maxNameSize) << volunteer->name << " will " << job->name << " with a score of " << desire << std::endl;
   }
   std::cout << std::endl;




   nodeNames.reserve(jobNames.size() + volunteerNames.size()); // preallocate memory
   nodeNames.insert(nodeNames.end(), jobNames.begin(), jobNames.end());
   nodeNames.insert(nodeNames.end(), volunteerNames.begin(), volunteerNames.end());

   auto g = matplot::graph(edges);
   //g->edge_labels(weights);
   g->node_labels(nodeNames);// array of labels for the nodes

   auto colors = std::vector<double>(nodeNames.size());
   size_t count = 0;
   std::generate(colors.begin(), colors.end(), [&]() { return (count++ < maxJobs)?0:1; });

   g->marker_colors(colors);

   auto y = std::vector<double>(nodeNames.size());
   count = 0;
   std::generate(y.begin(), y.end(), [&]() { return (count++ < maxJobs) ? -0.5+(count*0.1) : 0.5 + (count * 0.1); });

   g->y_data(y);
   auto line_widths = std::vector<double>();

   for (auto edgeIter = edgeWeights.begin(); edgeIter != edgeWeights.end(); edgeIter++) {
      line_widths.push_back(0.01);
   }
   for (auto assignmentIter = assignedEdgeIndexByWeight.begin(); assignmentIter != assignedEdgeIndexByWeight.end(); assignmentIter++) {
      auto assignedEdge = *assignmentIter;
      line_widths[assignedEdge] = 0.05;

      auto edge = edges[assignedEdge];
      auto volunteerId = edge.first;
      auto jobId = edge.second;
      auto volunteer = volunteerMap[volunteerId];
      auto job = jobMap[jobId];

      std::cout << std::setfill(' ') << std::setw(maxNameSize) << volunteer->name << " doing " << job->name << std::endl;

   }


   g->line_widths(line_widths);
//   g->edge_labels(edgeWeights);

   matplot::show();
}